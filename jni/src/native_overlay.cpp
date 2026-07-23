#include "native_overlay.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <cstdio>
#include <cstring>
#include <thread>
#include <atomic>
#include <android/log.h>
#include <errno.h>

#define TAG "physx_fb_overlay"

static std::thread g_overlay_thread;
static std::atomic<bool> g_overlay_running(false);
static int g_fb_fd = -1;
static void* g_fb_map = nullptr;
static size_t g_fb_map_size = 0;
static struct fb_var_screeninfo g_vinfo;
static struct fb_fix_screeninfo g_finfo;

static bool setup_framebuffer(const char** tried_path) {
    const char* paths[] = {"/dev/graphics/fb0", "/dev/fb0", nullptr};
    for (int i = 0; paths[i]; ++i) {
        const char* p = paths[i];
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Trying framebuffer path: %s", p);
        int fd = open(p, O_RDWR);
        if (fd < 0) {
            int e = errno;
            __android_log_print(ANDROID_LOG_WARN, TAG, "open('%s') failed: errno=%d (%s)", p, e, strerror(e));
            continue;
        }
        // got fd
        if (ioctl(fd, FBIOGET_FSCREENINFO, &g_finfo) < 0) {
            int e = errno;
            __android_log_print(ANDROID_LOG_WARN, TAG, "ioctl(FBIOGET_FSCREENINFO) on '%s' failed: errno=%d (%s)", p, e, strerror(e));
            close(fd);
            continue;
        }
        if (ioctl(fd, FBIOGET_VSCREENINFO, &g_vinfo) < 0) {
            int e = errno;
            __android_log_print(ANDROID_LOG_WARN, TAG, "ioctl(FBIOGET_VSCREENINFO) on '%s' failed: errno=%d (%s)", p, e, strerror(e));
            close(fd);
            continue;
        }
        size_t screensize = (size_t)g_vinfo.yres_virtual * g_finfo.line_length;
        void* map = mmap(nullptr, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            int e = errno;
            __android_log_print(ANDROID_LOG_WARN, TAG, "mmap on '%s' failed: errno=%d (%s)", p, e, strerror(e));
            close(fd);
            continue;
        }
        g_fb_fd = fd;
        g_fb_map = map;
        g_fb_map_size = screensize;
        if (tried_path) *tried_path = p;
        __android_log_print(ANDROID_LOG_INFO, TAG, "Framebuffer opened %s: resolution %dx%d bpp=%d line_length=%d", p, g_vinfo.xres, g_vinfo.yres, g_vinfo.bits_per_pixel, g_finfo.line_length);
        return true;
    }
    // none worked
    __android_log_print(ANDROID_LOG_ERROR, TAG, "All framebuffer open attempts failed");
    return false;
}

static void teardown_framebuffer() {
    if (g_fb_map && g_fb_map != MAP_FAILED) {
        munmap(g_fb_map, g_fb_map_size);
        g_fb_map = nullptr;
    }
    if (g_fb_fd >= 0) {
        close(g_fb_fd);
        g_fb_fd = -1;
    }
}

static void overlay_thread_func() {
    if (!g_fb_map) return;
    const int width = g_vinfo.xres;
    const int height = g_vinfo.yres;
    const int bpp = g_vinfo.bits_per_pixel; // usually 32 or 16
    const int line_len = g_finfo.line_length;

    // We'll draw a pulsing semi-opaque rectangle in the top-left corner
    const int rect_w = std::min(400, width);
    const int rect_h = std::min(200, height);

    int frame = 0;
    while (g_overlay_running.load()) {
        // compute color
        int alpha = 80 + (std::abs((frame % 60) - 30)); // 50..110
        uint8_t r = 0;
        uint8_t g = 255;
        uint8_t b = 0;

        if (bpp == 32) {
            for (int y = 0; y < rect_h; ++y) {
                uint8_t* base = (uint8_t*)g_fb_map + y * line_len;
                for (int x = 0; x < rect_w; ++x) {
                    uint8_t* px = base + x * 4;
                    uint8_t orig_b = px[0];
                    uint8_t orig_g = px[1];
                    uint8_t orig_r = px[2];
                    uint8_t out_r = (uint8_t)((alpha * r + (255 - alpha) * orig_r) / 255);
                    uint8_t out_g = (uint8_t)((alpha * g + (255 - alpha) * orig_g) / 255);
                    uint8_t out_b = (uint8_t)((alpha * b + (255 - alpha) * orig_b) / 255);
                    px[0] = out_b;
                    px[1] = out_g;
                    px[2] = out_r;
                }
            }
        } else if (bpp == 16) {
            for (int y = 0; y < rect_h; ++y) {
                uint8_t* base = (uint8_t*)g_fb_map + y * line_len;
                for (int x = 0; x < rect_w; ++x) {
                    uint8_t* px = base + x * 2;
                    uint16_t orig = px[0] | (px[1] << 8);
                    uint8_t orig_r5 = (orig >> 11) & 0x1F;
                    uint8_t orig_g6 = (orig >> 5) & 0x3F;
                    uint8_t orig_b5 = orig & 0x1F;
                    uint8_t or_r = (orig_r5 << 3) | (orig_r5 >> 2);
                    uint8_t or_g = (orig_g6 << 2) | (orig_g6 >> 4);
                    uint8_t or_b = (orig_b5 << 3) | (orig_b5 >> 2);
                    uint8_t out_r = (uint8_t)((alpha * r + (255 - alpha) * or_r) / 255);
                    uint8_t out_g = (uint8_t)((alpha * g + (255 - alpha) * or_g) / 255);
                    uint8_t out_b = (uint8_t)((alpha * b + (255 - alpha) * or_b) / 255);
                    uint16_t pr = (out_r >> 3) & 0x1F;
                    uint16_t pg = (out_g >> 2) & 0x3F;
                    uint16_t pb = (out_b >> 3) & 0x1F;
                    uint16_t packed = (pr << 11) | (pg << 5) | pb;
                    px[0] = packed & 0xFF;
                    px[1] = (packed >> 8) & 0xFF;
                }
            }
        } else {
            // unsupported bpp
            __android_log_print(ANDROID_LOG_WARN, TAG, "Unsupported bits_per_pixel=%d", bpp);
            break;
        }

        frame++;
        usleep(100000); // 100ms
    }
}

bool StartFramebufferOverlay() {
    if (g_overlay_running.load()) return true;
    const char* path = nullptr;
    if (!setup_framebuffer(&path)) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to open framebuffer device - check permissions, SELinux, and device paths");
        return false;
    }
    g_overlay_running.store(true);
    g_overlay_thread = std::thread(overlay_thread_func);
    __android_log_print(ANDROID_LOG_INFO, TAG, "Framebuffer overlay started on %s", path?path:"(unknown)");
    return true;
}

void StopFramebufferOverlay() {
    if (!g_overlay_running.load()) return;
    g_overlay_running.store(false);
    if (g_overlay_thread.joinable()) g_overlay_thread.join();
    teardown_framebuffer();
    __android_log_print(ANDROID_LOG_INFO, TAG, "Framebuffer overlay stopped");
}
