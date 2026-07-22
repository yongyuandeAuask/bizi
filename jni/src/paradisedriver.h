#ifndef PARADISE_DRIVER_USER_H
#define PARADISE_DRIVER_USER_H

#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdint.h>
#include <stddef.h>
#include <mutex>

#define PARADISE_TOUCH_MODE_FOPS_HOOK 0
#define PARADISE_TOUCH_MODE_KPROBE 1
#define PARADISE_TOUCH_MODE_DIRECT 2

// define __NR_reboot if not defined
#ifndef __NR_reboot
#if defined(__aarch64__) || defined(__arm__)
#define __NR_reboot 142
#elif defined(__x86_64__)
#define __NR_reboot 169
#elif defined(__i386__)
#define __NR_reboot 88
#elif defined(SYS_reboot)
#define __NR_reboot SYS_reboot
#else
#error "__NR_reboot not defined and cannot be determined for this architecture"
#endif
#endif

// Magic numbers for reboot syscall to install fd
#define PARADISE_INSTALL_MAGIC1 0xDEADBEEF
#define PARADISE_INSTALL_MAGIC2 0xCAFEBABE

// Driver name in /proc/self/fd
#define PARADISE_DRIVER_NAME "[paradise_driver]"

// IOCTL command structures
struct paradise_get_pid_cmd
{
    pid_t pid;
    char name[256];
};

struct paradise_get_module_base_cmd
{
    pid_t pid;
    char name[256];
    uintptr_t base;
    int vm_flag;
};

struct paradise_memory_cmd
{
    pid_t pid;
    uintptr_t src_va;
    uintptr_t dst_va;
    size_t size;
    uintptr_t phy_addr;
};

struct paradise_memory_ioremap_cmd
{
    pid_t pid;
    uintptr_t src_va;
    uintptr_t dst_va;
    size_t size;
    uintptr_t phy_addr;
    int prot;
};

struct paradise_gyro_config_cmd
{
    int enable;
    uint32_t type_mask;
    float x;
    float y;
};

#define PARADISE_GYRO_MASK_GYRO (1u << 0)
#define PARADISE_GYRO_MASK_UNCAL (1u << 1)
#define PARADISE_GYRO_MASK_ALL (PARADISE_GYRO_MASK_GYRO | PARADISE_GYRO_MASK_UNCAL)

struct paradise_touch_down_cmd
{
    int slot;
    int x;
    int y;
};

struct paradise_touch_move_cmd
{
    int slot;
    int x;
    int y;
};

struct paradise_touch_up_cmd
{
    int slot;
};

struct paradise_touch_mode_cmd
{
    int mode;
};

// IOCTL commands
#define PARADISE_IOCTL_GET_PID _IOWR('W', 11, struct paradise_get_pid_cmd)                               // 查找进程
#define PARADISE_IOCTL_GET_MODULE_BASE _IOWR('W', 10, struct paradise_get_module_base_cmd)               // 获取模块基地址
#define PARADISE_IOCTL_READ_MEMORY _IOWR('W', 9, struct paradise_memory_cmd)                             // 硬件读
#define PARADISE_IOCTL_WRITE_MEMORY _IOWR('W', 12, struct paradise_memory_cmd)                           // 硬件写
#define PARADISE_IOCTL_READ_MEMORY_IOREMAP _IOWR('W', 16, struct paradise_memory_ioremap_cmd)            // 内核映射读
#define PARADISE_IOCTL_WRITE_MEMORY_IOREMAP _IOWR('W', 17, struct paradise_memory_ioremap_cmd)           // 内核映射写
#define PARADISE_IOCTL_GYRO_CONFIG _IOWR('W', 21, struct paradise_gyro_config_cmd)                       // 陀螺仪
#define PARADISE_IOCTL_TOUCH_DOWN _IOWR('W', 22, struct paradise_touch_down_cmd)                         // 触摸按下
#define PARADISE_IOCTL_TOUCH_MOVE _IOWR('W', 23, struct paradise_touch_move_cmd)                         // 触摸移动
#define PARADISE_IOCTL_TOUCH_UP _IOWR('W', 24, struct paradise_touch_up_cmd)                             // 触摸抬起
#define PARADISE_IOCTL_TOUCH_SET_MODE _IOWR('W', 25, struct paradise_touch_mode_cmd)                     // 设置触摸模式

struct TouchState
{
    uint32_t device_min_x, device_max_x;
    uint32_t device_min_y, device_max_y;
    uint32_t screen_width, screen_height;
    int screen_orientation; // 0=正常, 1=90度, 2=180度, 3=270度
    float device_scale_x, device_scale_y;
    bool initialized;

    TouchState() : device_min_x(0), device_max_x(0), device_min_y(0), device_max_y(0),
                   screen_width(0), screen_height(0), screen_orientation(0),
                   device_scale_x(0.0f), device_scale_y(0.0f), initialized(false) {}
};

static TouchState g_touch_state;

// 检查是否为触摸设备
inline bool is_touch_device(int fd)
{
    // 检查设备是否支持多点触控
    unsigned long abs_bits[(ABS_MAX + sizeof(long) * 8 - 1) / (sizeof(long) * 8)] = {0};

    if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_bits)), abs_bits) < 0)
    {
        return false;
    }

    // 检查是否支持必要的多点触控事件
    auto test_bit = [](int bit, const unsigned long *array) -> bool
    {
        return (array[bit / (sizeof(long) * 8)] >> (bit % (sizeof(long) * 8))) & 1;
    };

    bool has_slot = test_bit(ABS_MT_SLOT, abs_bits);
    bool has_tracking_id = test_bit(ABS_MT_TRACKING_ID, abs_bits);
    bool has_pos_x = test_bit(ABS_MT_POSITION_X, abs_bits);
    bool has_pos_y = test_bit(ABS_MT_POSITION_Y, abs_bits);

    return has_slot && has_tracking_id && has_pos_x && has_pos_y;
}

// 获取触摸设备坐标范围
inline bool get_touch_device_range(uint32_t *min_x, uint32_t *max_x, uint32_t *min_y, uint32_t *max_y)
{
    DIR *input_dir = opendir("/dev/input/");
    if (!input_dir)
    {
        return false;
    }

    struct dirent *file_entry;
    bool found = false;

    while ((file_entry = readdir(input_dir)) != nullptr)
    {
        std::string filename = file_entry->d_name;
        if (filename.find("event") != 0)
        {
            continue;
        }

        std::string path = "/dev/input/" + filename;
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1)
        {
            continue;
        }

        if (is_touch_device(fd))
        {
            // 获取坐标范围
            struct input_absinfo abs_x, abs_y;
            if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &abs_x) == 0 &&
                ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &abs_y) == 0)
            {
                *min_x = abs_x.minimum;
                *max_x = abs_x.maximum;
                *min_y = abs_y.minimum;
                *max_y = abs_y.maximum;
                found = true;
                // printf("[√] get_touch_device_range: found device %s, range=[%u,%u]x[%u,%u]\n",
                //        path.c_str(), *min_x, *max_x, *min_y, *max_y);
            }
            close(fd);
            break;
        }

        close(fd);
    }

    closedir(input_dir);
    return found;
}

// 坐标转换辅助函数
inline void convert_screen_to_physical(int screen_x, int screen_y, int *phys_x, int *phys_y)
{
    int physical_x = 0, physical_y = 0;

    switch (g_touch_state.screen_orientation)
    {
    case 0: // 正常
        physical_x = screen_x;
        physical_y = screen_y;
        break;
    case 1: // 90度
        physical_x = g_touch_state.screen_height - screen_y;
        physical_y = screen_x;
        break;
    case 3: // 270度
        physical_x = screen_y;
        physical_y = g_touch_state.screen_width - screen_x;
        break;
    default: // 180度
        physical_x = g_touch_state.screen_height - screen_y;
        physical_y = g_touch_state.screen_width - screen_x;
        break;
    }

    *phys_x = physical_x;
    *phys_y = physical_y;
}

inline void convert_physical_to_device(int phys_x, int phys_y, uint32_t *dev_x, uint32_t *dev_y)
{
    if (g_touch_state.device_scale_x > 0.0f && g_touch_state.device_scale_y > 0.0f)
    {
        *dev_x = (uint32_t)((float)phys_x / g_touch_state.device_scale_x);
        *dev_y = (uint32_t)((float)phys_y / g_touch_state.device_scale_y);
    }
    else
    {
        // scale 未初始化，直接使用物理坐标
        *dev_x = (uint32_t)phys_x;
        *dev_y = (uint32_t)phys_y;
    }

    // 限幅到设备范围内
    if (*dev_x < g_touch_state.device_min_x)
        *dev_x = g_touch_state.device_min_x;
    if (*dev_x > g_touch_state.device_max_x)
        *dev_x = g_touch_state.device_max_x;
    if (*dev_y < g_touch_state.device_min_y)
        *dev_y = g_touch_state.device_min_y;
    if (*dev_y > g_touch_state.device_max_y)
        *dev_y = g_touch_state.device_max_y;
}

class Paradise_hook_driver
{
private:
    pid_t pid;
    int fd;

    std::mutex driver_lock;

    int scan_driver_fd()
    {
        DIR *dir = opendir("/proc/self/fd");
        if (!dir)
        {
            return -1;
        }

        struct dirent *entry;
        char link_path[256];
        char target[256];
        ssize_t len;

        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            int fd_num = atoi(entry->d_name);
            if (fd_num < 0)
            {
                continue;
            }

            snprintf(link_path, sizeof(link_path), "/proc/self/fd/%d", fd_num);
            len = readlink(link_path, target, sizeof(target) - 1);
            if (len < 0)
            {
                continue;
            }
            target[len] = '\0';

            // Check if this is our driver
            if (strstr(target, PARADISE_DRIVER_NAME) != NULL)
            {
                closedir(dir);
                return fd_num;
            }
        }

        closedir(dir);
        return -1;
    }

    int install_driver_fd()
    {
        int fd = -1;

        long ret = syscall(__NR_reboot, PARADISE_INSTALL_MAGIC1, PARADISE_INSTALL_MAGIC2, 0, &fd);

        if (fd < 0)
        {
            printf("install_driver_fd: fd not installed, ret=%ld, errno=%d\n", ret, errno);
            return -1;
        }

        printf("install_driver_fd: fd=%d installed successfully\n", fd);
        return fd;
    }

    int get_driver_fd()
    {
        int fd = scan_driver_fd();
        if (fd >= 0)
        {
            return fd;
        }

        fd = install_driver_fd();
        if (fd >= 0)
        {
            return fd;
        }

        return -1;
    }

public:
    Paradise_hook_driver()
    {
        fd = get_driver_fd();

        if (fd < 0)
        {
            printf("无法找到驱动\n");
            exit(1);
        }

        printf("识别到Paradise Driver | fd %d\n", fd);
    }

    ~Paradise_hook_driver()
    {
        if (fd >= 0)
        {
            close(fd);
            fd = -1;
        }
    }

    pid_t get_pid(const char *name)
    {
        if (fd < 0)
        {
            return false;
        }

        struct paradise_get_pid_cmd cmd = {0, ""};

        strncpy(cmd.name, name, sizeof(cmd.name) - 1);

        if (ioctl(fd, PARADISE_IOCTL_GET_PID, &cmd) != 0)
        {
            return 0;
        }

        return cmd.pid;
    }

    void initialize(pid_t target_pid)
    {
        this->pid = target_pid;
    }

    uintptr_t get_module_base(const char *name)
    {
        if (this->pid <= 0)
        {
            return 0;
        }

        struct paradise_get_module_base_cmd cmd = {this->pid, "", 0, 0};

        strncpy(cmd.name, name, sizeof(cmd.name) - 1);
        if (ioctl(fd, PARADISE_IOCTL_GET_MODULE_BASE, &cmd) != 0)
            return 0;
        return cmd.base;
    }

    // bool gyro_update(float x, float y, uint32_t type_mask = PARADISE_GYRO_MASK_ALL, bool enable = true)
    bool gyro_update(float x, float y, uint32_t type_mask = PARADISE_GYRO_MASK_ALL, bool enable = true)
    {
        if (fd < 0)
        {
            return false;
        }

        struct paradise_gyro_config_cmd cmd{};

        cmd.enable = enable ? 1 : 0;
        cmd.type_mask = type_mask ? type_mask : PARADISE_GYRO_MASK_ALL;
        cmd.x = x;
        cmd.y = y;
        return ioctl(fd, PARADISE_IOCTL_GYRO_CONFIG, &cmd) == 0;
    }

    bool read(uintptr_t addr, void *buffer, size_t size)
    {
        if (fd < 0)
        {
            return false;
        }

        if (buffer == nullptr || this->pid <= 0)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(this->driver_lock);

        struct paradise_memory_ioremap_cmd cmd = {};

        cmd.pid = this->pid;
        cmd.src_va = addr;
        cmd.dst_va = (uintptr_t)buffer;
        cmd.size = size;
        cmd.prot = 2; // WMT_NORMAL_NC

        return ioctl(fd, PARADISE_IOCTL_READ_MEMORY_IOREMAP, &cmd) == 0;
    }

    template <typename T>
    T read(uintptr_t addr)
    {
        T res{};
        if (this->read(addr, &res, sizeof(T)))
            return res;
        return {};
    }

    bool write(uintptr_t addr, void *buffer, size_t size)
    {
        if (fd < 0)
        {
            return false;
        }

        if (buffer == nullptr || this->pid <= 0)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(this->driver_lock);

        struct paradise_memory_ioremap_cmd cmd = {};

        cmd.pid = this->pid;
        cmd.src_va = addr;
        cmd.dst_va = (uintptr_t)buffer;
        cmd.size = size;
        cmd.prot = 2; // WMT_NORMAL_NC

        return ioctl(fd, PARADISE_IOCTL_WRITE_MEMORY_IOREMAP, &cmd) == 0;
    }

    template <typename T>
    T write(uintptr_t addr, T value)
    {
        return this->write(addr, &value, sizeof(T));
    }

    bool read_safe(uintptr_t addr, void *buffer, size_t size)
    {
        if (fd < 0)
        {
            return false;
        }

        if (buffer == nullptr || this->pid <= 0)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(this->driver_lock);

        struct paradise_memory_cmd cmd = {};

        cmd.pid = this->pid;
        cmd.src_va = addr;
        cmd.dst_va = (uintptr_t)buffer;
        cmd.size = size;

        return ioctl(fd, PARADISE_IOCTL_READ_MEMORY, &cmd) == 0;
    }

    template <typename T>
    T read_safe(uintptr_t addr)
    {
        T res{};
        if (this->read_safe(addr, &res, sizeof(T)))
            return res;
        return {};
    }

    bool write_safe(uintptr_t addr, void *buffer, size_t size)
    {
        if (fd < 0)
        {
            return false;
        }

        if (buffer == nullptr || this->pid <= 0)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(this->driver_lock);

        struct paradise_memory_cmd cmd = {};

        cmd.pid = this->pid;
        cmd.src_va = addr;
        cmd.dst_va = (uintptr_t)buffer;
        cmd.size = size;

        return ioctl(fd, PARADISE_IOCTL_WRITE_MEMORY, &cmd) == 0;
    }

    template <typename T>
    T write_safe(uintptr_t addr, T value)
    {
        return this->write_safe(addr, &value, sizeof(T));
    }

    bool touch_init(int width, int height)
    {
        if (width <= 0 || height <= 0)
        {
            return false;
        }

        if (!g_touch_state.initialized)
        {
            uint32_t min_x, max_x, min_y, max_y;
            if (get_touch_device_range(&min_x, &max_x, &min_y, &max_y))
            {
                g_touch_state.device_min_x = min_x;
                g_touch_state.device_max_x = max_x;
                g_touch_state.device_min_y = min_y;
                g_touch_state.device_max_y = max_y;

                if (width > height)
                {
                    g_touch_state.screen_width = width;
                    g_touch_state.screen_height = height;
                }
                else
                {
                    g_touch_state.screen_width = height;
                    g_touch_state.screen_height = width;
                }

                if (g_touch_state.device_max_x > g_touch_state.device_min_x)
                {
                    g_touch_state.device_scale_x = (float)g_touch_state.screen_height / (float)g_touch_state.device_max_x;
                }
                if (g_touch_state.device_max_y > g_touch_state.device_min_y)
                {
                    g_touch_state.device_scale_y = (float)g_touch_state.screen_width / (float)g_touch_state.device_max_y;
                }

                g_touch_state.initialized = true;
            }
        }
        return g_touch_state.initialized;
    }

    bool touch_down(int slot, int x, int y)
    {
        if (fd < 0)
        {
            return false;
        }

        uint32_t dev_x, dev_y;

        // 如果触摸已初始化，进行坐标转换
        if (g_touch_state.initialized)
        {
            int phys_x, phys_y;
            convert_screen_to_physical(x, y, &phys_x, &phys_y);
            convert_physical_to_device(phys_x, phys_y, &dev_x, &dev_y);
        }
        else
        {
            // 未初始化，直接使用传入坐标作为设备坐标
            dev_x = (uint32_t)x;
            dev_y = (uint32_t)y;
        }

        paradise_touch_down_cmd cmd = {};
        cmd.slot = slot;
        cmd.x = dev_x;
        cmd.y = dev_y;

        return ioctl(fd, PARADISE_IOCTL_TOUCH_DOWN, &cmd) == 0;
    }

    bool touch_move(int slot, int x, int y)
    {
        if (fd < 0)
        {
            return false;
        }

        uint32_t dev_x, dev_y;

        // 如果触摸已初始化，进行坐标转换
        if (g_touch_state.initialized)
        {
            int phys_x, phys_y;
            convert_screen_to_physical(x, y, &phys_x, &phys_y);
            convert_physical_to_device(phys_x, phys_y, &dev_x, &dev_y);
        }
        else
        {
            // 未初始化，直接使用传入坐标作为设备坐标
            dev_x = (uint32_t)x;
            dev_y = (uint32_t)y;
        }

        paradise_touch_move_cmd cmd = {};
        cmd.slot = slot;
        cmd.x = dev_x;
        cmd.y = dev_y;

        return ioctl(fd, PARADISE_IOCTL_TOUCH_MOVE, &cmd) == 0;
    }

    bool touch_up(int slot)
    {
        if (fd < 0)
        {
            return false;
        }

        paradise_touch_up_cmd cmd = {};
        cmd.slot = slot;

        return ioctl(fd, PARADISE_IOCTL_TOUCH_UP, &cmd) == 0;
    }

    bool touch_set_mode(int mode)
    {
        if (fd < 0)
        {
            return false;
        }

        paradise_touch_mode_cmd cmd = {};
        cmd.mode = mode;

        return ioctl(fd, PARADISE_IOCTL_TOUCH_SET_MODE, &cmd) == 0;
    }
};

#endif // PARADISE_DRIVER_USER_H