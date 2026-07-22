#include "imgui.h"
#include "图片调用.h"
#include "辅助类.h"
#include <linux/input.h>
#include <GifLoader.h> // Ui中引入gif加载器
#include "UI/函数库.h"
#include "UI/背景图.h"
#include "UI/系统粒子.h"
#include "音频/OpenSLWavPlayer.h"
#include "手持美化.h"
extern 绘制 绘制;
 bool 悬浮窗 = true;
 static bool g_showCpuDashboard = true; // 前缀g_区分全局变量，默认开启

static bool 悬浮球 = true;
static bool showWindow = true; // 默认显示窗口
static bool is_connected = false;
std::atomic<bool> 美化中 = false;
std::thread 美化线程;
std::atomic<bool> 正在初始化(false);
static bool particleSystemInitialized = false;
static bool 模型初始化 = false;/*模型漏打*/
static bool 绘制模型 = false;
void 模型() {
    if (模型初始化) {
        return;
    }
    if (!绘制.按钮.手动开启) {
        ReadGame::Initialization();
        模型初始化 = true;
        return;
    }
}

int 数据() {
    DIR *dir = opendir("/dev/input/");
    if (!dir) {
        perror("无法打开/dev/input/目录");
        return -1;
    }

    struct dirent *ptr = NULL;
    int count = 0;
    
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event")) {
            count++;
        }
    }
    
    closedir(dir);
    return count ? count : -1;
}

int 音量() {
    int EventCount = 数据();
    if (EventCount < 0) {
        fprintf(stderr, "未找到输入设备\n");
        return -1;
    }

    int *fdArray = (int *)malloc(EventCount * sizeof(int));
    if (!fdArray) {
        perror("内存分配失败");
        return -1;
    }

    // 初始化文件描述符数组
    for (int i = 0; i < EventCount; i++) {
        fdArray[i] = -1;  // 初始化为无效值
    }

    // 打开设备文件
    for (int i = 0; i < EventCount; i++) {
        char temp[128];
        snprintf(temp, sizeof(temp), "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
        if (fdArray[i] < 0) {
            perror("无法打开输入设备");
            // 继续尝试其他设备而不是直接返回
            continue;
        }
    }

    struct input_event ev;
    int count = 0;

    while (1) {
        for (int i = 0; i < EventCount; i++) {
            if (fdArray[i] < 0) continue;  // 跳过无效的文件描述符

            memset(&ev, 0, sizeof(ev));
            ssize_t bytes = read(fdArray[i], &ev, sizeof(ev));
            
            if (bytes < 0) {
                if (errno != EAGAIN) {  // 非阻塞模式下EAGAIN是正常情况
                    perror("读取输入事件失败");
                    close(fdArray[i]);
                    fdArray[i] = -1;
                }
                continue;
            }

            if (bytes == sizeof(ev) && ev.type == EV_KEY) {
                if (ev.code == KEY_VOLUMEUP && ev.value == 1) {
                    悬浮窗 = true;
                } else if (ev.code == KEY_VOLUMEDOWN && ev.value == 1) {
                    悬浮窗 = false;
                }
            }
        }
        usleep(5000);  // 稍微增加延迟减少CPU占用
    }

    // 清理资源 (虽然无限循环中不会执行到这里)
    for (int i = 0; i < EventCount; i++) {
        if (fdArray[i] >= 0) {
            close(fdArray[i]);
        }
    }
    free(fdArray);

    return 0;
}
static bool 自瞄控件 = false;
static int style_idx = 0;
// ImVec2 Pos; //窗口位置
ImVec2 Pos2;
ImVec2 windowSize, windowSize_max;
static bool IsBall = false;
static bool 窗口状态 = false;
static bool show_ChildMenu1 = true;
static bool show_ChildMenu2 = false;
static bool show_ChildMenu3 = false;
static bool show_ChildMenu4 = false;
static bool show_ChildMenu5 = false;

static int 自瞄窗口 = 0;
static int 配置窗口 = 0;
static int 物资窗口 = 0;
static bool 广角设置 = false;

ImU32 c_透明 = IM_COL32(0, 0, 0, 0);
ImU32 c_fafafa = IM_COL32(250, 250, 250, 255);
ImU32 c_cccccc = IM_COL32(204, 204, 204, 255);
ImU32 c_c2c2c2 = IM_COL32(194, 194, 194, 255);
ImU32 c_23292e = IM_COL32(35, 41, 46, 255);
ImU32 c_4023292e = IM_COL32(35, 41, 46, 125);
ImU32 c_eeeeee = IM_COL32(0xee, 0xee, 0xee, 255);
ImU32 c_2f363c = IM_COL32(47, 54, 60, 255);
ImU32 c_402f363c = IM_COL32(47, 54, 60, 125);
ImU32 c_DAB123 = IM_COL32(218, 177, 35, 255);
ImU32 c_DCD4CA = IM_COL32(220, 212, 202, 255);
ImU32 c_2C88E2 = IM_COL32(44, 136, 226, 255);
ImU32 c_2C88E240 = IM_COL32(44, 136, 226, 128);
ImU32 c_545D6D = IM_COL32(84, 93, 109, 255);
ImU32 c_545D6D40 = IM_COL32(84, 93, 109, 128);
ImU32 c_FFD49F = IM_COL32(255, 212, 159, 255);
ImU32 c_FFD49F80 = IM_COL32(255, 212, 159, 128);
ImU32 c_16b777 = IM_COL32(0x16, 0xB7, 0x77, 255);
ImU32 c_16b77780 = IM_COL32(0x16, 0xB7, 0x77, 128);
ImU32 c_11243B = IM_COL32(0x11, 0x24, 0x3B, 255);
ImU32 c_11243B80 = IM_COL32(0x11, 0x24, 0x3B, 128);
ImU32 c_31bdec = IM_COL32(0x31, 0xbd, 0xec, 255);
ImU32 c_558FFD = IM_COL32(0x55, 0x8F, 0xFD, 255);
ImU32 c_81A4DC = IM_COL32(0x81, 0xa4, 0xdc, 255);
ImVec4 透明ImVec4 = ImVec4(0, 0, 0, 0);
ImVec4 黑色ImVec4 = ImVec4(0, 0, 0, 1.0f);
ImVec4 灰色ImVec4 = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
ImVec4 灰色ImVec440 = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
ImVec4 灰色ImVec480 = ImVec4(0.1f, 0.1f, 0.1f, 0.75f);
ImVec4 亮灰色ImVec4 = ImVec4(0.5, 0.5, 0.5, 1);
ImVec4 亮黄色ImVec4 = ImVec4(1.0f, 0.831f, 0.623f, 1.0f);
ImVec4 半透明黑色ImVec4 = ImVec4(0.184314f, 0.211765f, 0.235294f, 0.5f);
ImVec4 半透明黑色ImVec480 = ImVec4(0.184314f, 0.211765f, 0.235294f, 0.75f);
ImVec4 ImVec423292e = ImVec4(0.137255f, 0.160784f, 0.180392f, 1.0f);
ImVec4 ImVec80423292e = ImVec4(0.137255f, 0.160784f, 0.180392f, 0.75f);
ImVec4 ImVec40423292e = ImVec4(0.137255f, 0.160784f, 0.180392f, 0.5f);
ImVec4 白色ImVec4 = ImVec4(1, 1, 1, 1);
ImVec4 ImVec4fafafa = ImVec4(0xfa / 255.0f, 0xfa / 255.0f, 0xfa / 255.0f, 1.0f);
ImVec4 ImVec431bdec = ImVec4(0x31 / 255.0f, 0xbd / 255.0f, 0xec / 255.0f, 1.0f);
ImVec4 ImVec416b777 = ImVec4(0x16 / 255.0f, 0xB7 / 255.0f, 0x77 / 255.0f, 1.0f);
ImVec4 ImVec4c2c2c2 = ImVec4(0xC2 / 255.0f, 0xC2 / 255.0f, 0xC2 / 255.0f, 1.0f);
ImVec4 ImVec42C88E2 = ImVec4(0x2C / 255.0f, 0x88 / 255.0f, 0xE2 / 255.0f, 1.0f);
ImVec4 ImVec411243B = ImVec4(0x11 / 255.0f, 0x24 / 255.0f, 0x3B / 255.0f, 1.0f);
ImVec4 ImVec411243B80 = ImVec4(0x11 / 255.0f, 0x24 / 255.0f, 0x3B / 255.0f, 0.75f);
ImVec4 ImVec411243B40 = ImVec4(0x11 / 255.0f, 0x24 / 255.0f, 0x3B / 255.0f, 0.5f);
ImVec4 ImVec4f3c258 = ImVec4(0xf3 / 255.0f, 0xc2 / 255.0f, 0x58 / 255.0f, 1.0f);
ImVec4 ImVec4eeeeee = ImVec4(0xee / 255.0f, 0xee / 255.0f, 0xee / 255.0f, 1.0f);
ImVec4 ImVec4558FFD = ImVec4(0x55 / 255.0f, 0x8f / 255.0f, 0xff / 255.0f, 1.0f);
ImVec4 ImVec481A4DC = ImVec4(0x81 / 255.0f, 0xa4 / 255.0f, 0xdc / 255.0f, 1.0f);

void 布局::绘制悬浮窗() {
    ImVec2 pos;
    ImDrawList* draw;
    drawBegin();
    static bool firstTime = true;
    if (firstTime) {
        OpenSLWavPlayer::play(Go, sizeof(Go));
        firstTime = false;
    }
    if (模型初始化){
    ReadGame::ReadGameData();
    }
    if (绘制模型){
    // 执行模型绘制相关的核心逻辑
    ReadGame::Raycast();
    }
    if (广角设置) {
        long 广角地址 = 绘制.读写.getPtr64(绘制.地址.自身地址 + 0x1198);
        绘制.读写.WriteFloat(广角地址 + 0x2f4, 绘制.按钮.第三人称);
    }
static ImVec2 cpuCirclePos = ImVec2(100, 100);
static float cpuUsage = 0.0f;
static auto lastTime = std::chrono::high_resolution_clock::now();
static unsigned long long lastUser = 0, lastNice = 0, lastSystem = 0, lastIdle = 0;
if (g_showCpuDashboard) {
    auto now = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count() >= 500) {
        lastTime = now;
        FILE* file = fopen("/proc/stat", "r");
        if (file) {
            unsigned long long user, nice, system, idle;
            if (fscanf(file, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle) == 4) {
                if (lastUser != 0) {
                    unsigned long long total = user + nice + system + idle;
                    unsigned long long diffTotal = total - (lastUser + lastNice + lastSystem + lastIdle);
                    unsigned long long diffIdle = idle - lastIdle;
                    cpuUsage = diffTotal > 0 ? 100.0f * (diffTotal - diffIdle) / diffTotal : 0.0f;
                }
                lastUser = user;
                lastNice = nice;
                lastSystem = system;
                lastIdle = idle;
            }
            fclose(file);
        }
    }
ImGui::SetNextWindowPos(cpuCirclePos, ImGuiCond_FirstUseEver);
ImGui::SetNextWindowSize(ImVec2(140, 140), ImGuiCond_Always);
ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
ImGui::Begin("##CPUCircle", nullptr,
             ImGuiWindowFlags_NoTitleBar |
             ImGuiWindowFlags_NoResize |
             ImGuiWindowFlags_NoScrollbar |
             ImGuiWindowFlags_NoCollapse |
             ImGuiWindowFlags_NoBackground);  
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 center = ImVec2(windowPos.x + 70.0f, windowPos.y + 70.0f);
    const float radius = 60.0f;
    draw_list->AddCircleFilled(center, radius + 5.0f, IM_COL32(8, 12, 22, 180), 64);
    draw_list->PathClear();
    draw_list->PathArcTo(center, radius + 4.0f, 0.0f, IM_PI * 2.0f, 48);
    draw_list->PathStroke(IM_COL32(40, 55, 75, 160), false, 3.0f);
    draw_list->AddCircleFilled(center, radius, IM_COL32(12, 16, 26, 245), 64);
    draw_list->PathClear();
    draw_list->PathArcTo(center, radius - 1.0f, 0.0f, IM_PI * 2.0f, 48);
    draw_list->PathStroke(IM_COL32(40, 180, 200, 40), false, 1.0f);
    float startAngle = -IM_PI * 0.5f;
    float endAngle   = startAngle + (cpuUsage / 100.0f) * 2 * IM_PI;
    ImU32 progressColor;
    if (cpuUsage < 30.0f) {
        progressColor = IM_COL32(60, 210, 170, 255);
    } else if (cpuUsage < 70.0f) {
        progressColor = IM_COL32(230, 190, 90, 255);
    } else {
        progressColor = IM_COL32(230, 85, 95, 255);
    }
    draw_list->PathClear();
    draw_list->PathArcTo(center, radius - 5.0f, startAngle, endAngle, 40);
    draw_list->PathStroke(progressColor, false, 7.0f);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.0f%%", cpuUsage);
    ImVec2 txtSize = ImGui::CalcTextSize(buf);
    ImVec2 txtPos = ImVec2(center.x - txtSize.x * 0.5f, center.y - txtSize.y * 0.5f - 6);
    draw_list->AddText(txtPos, IM_COL32(235, 240, 248, 255), buf);
    txtSize = ImGui::CalcTextSize("CPU");
    txtPos  = ImVec2(center.x - txtSize.x * 0.5f, center.y + 8);
    draw_list->AddText(txtPos, IM_COL32(130, 145, 165, 220), "CPU");
    ImGui::InvisibleButton("##CpuDrag", ImVec2(140, 140));
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        cpuCirclePos.x += ImGui::GetIO().MouseDelta.x;
        cpuCirclePos.y += ImGui::GetIO().MouseDelta.y;
        ImGui::SetWindowPos(cpuCirclePos, ImGuiCond_Always);
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

    if (绘制.按钮.绘制) {
        绘制.运行绘制();
    }    
    if (绘制.自瞄.自瞄控件) {
        ImGui::SetNextWindowSize({200, 200});
        if (ImGui::Begin("自瞄控件", &自瞄控件, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
            auto Pos = ImGui::GetWindowPos();

            static bool isMouseDown = false;
            static bool wasMousePressed = false;
            static bool isTe = false;
            static ImVec2 mousePressPos;
            bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            bool windowHovered = ImGui::IsWindowHovered();

            if (mouseDown && !isMouseDown && windowHovered && ImGui::IsMouseHoveringRect(Pos, {Pos.x + 100, Pos.y + 100})) {
                isMouseDown = true;
                wasMousePressed = true;
                mousePressPos = ImGui::GetMousePos();
            } else if (!mouseDown && isMouseDown && wasMousePressed) {
                ImVec2 mouseReleasePos = ImGui::GetMousePos();
                if (mousePressPos.x == mouseReleasePos.x && mousePressPos.y == mouseReleasePos.y) {
                    isTe = !isTe;
                }

                isMouseDown = false;
                wasMousePressed = false;
            }

            if (isTe) {
                绘制.自瞄.初始化 = true;
                ImGui::GetWindowDrawList()->AddImage(手持图片[4].textureId, {Pos.x + 20, Pos.y + 20}, {Pos.x + 120, Pos.y + 120});
            } else {
                绘制.自瞄.初始化 = false;
                ImGui::GetWindowDrawList()->AddImage(手持图片[3].textureId, {Pos.x + 20, Pos.y + 20}, {Pos.x + 120, Pos.y + 120});
            }
        }
        ImGui::End();
    }
   
if (悬浮窗) {
    ImVec2 mainWindowPos = 绘制.Pos;
    ImVec2 mainWindowSize = ImVec2(绘制.winWidth, 绘制.winHeith);
    
    float imageWidth = 200.0f;
    float imageHeight = 200.0f;
    
    float leftOffsetX = -50.0f;
    float leftOffsetY = mainWindowSize.y;
    
    float rightOffsetX = mainWindowSize.x - 170.0f;
    float rightOffsetY = mainWindowSize.y;
    
    ImVec2 bottomLeftPos = ImVec2(mainWindowPos.x + leftOffsetX, mainWindowPos.y + leftOffsetY);
    
    // if (手持图片.size() > 0) {
        // ImGui::GetBackgroundDrawList()->AddImage(手持图片[200].textureId, bottomLeftPos, ImVec2(bottomLeftPos.x + imageWidth, bottomLeftPos.y + imageHeight));
    // }    
    ImVec2 bottomRightPos = ImVec2(mainWindowPos.x + rightOffsetX, mainWindowPos.y + rightOffsetY);
    
    // if (手持图片.size() > 1) {
        // ImGui::GetBackgroundDrawList()->AddImage(手持图片[200].textureId, bottomRightPos, ImVec2(bottomRightPos.x + imageWidth, bottomRightPos.y + imageHeight));
    // }
}
    static bool 动画进行中 = false;
    static float 动画进度 = 0.0f; 
    static float 动画持续时间 = 0.6f; 
    static ImVec2 窗口最终尺寸 = ImVec2(990.0f, 730.0f); 
    screen_config();
    ImVec2 屏幕中心;
    if (Orientation == 0 || Orientation == 2) {
        屏幕中心 = ImVec2(displayInfo.width / 2.0f, displayInfo.height / 2.0f);
    } else {
        屏幕中心 = ImVec2(abs_ScreenX / 2.0f, abs_ScreenY / 2.0f);
    }
    ImVec2 目标位置 = ImVec2(屏幕中心.x - 窗口最终尺寸.x / 2.0f, 屏幕中心.y - 窗口最终尺寸.y / 2.0f);
    ImVec2 收缩位置 = ImVec2(屏幕中心.x - 窗口最终尺寸.x / 2.0f, -窗口最终尺寸.y);
    
    if (悬浮窗 && 动画进度 < 1.0f) {
        动画进度 += ImGui::GetIO().DeltaTime / 动画持续时间;
        if (动画进度 > 1.0f) 动画进度 = 1.0f;
        动画进行中 = true;
    } else if (!悬浮窗 && 动画进度 > 0.0f) {
        动画进度 -= ImGui::GetIO().DeltaTime / 动画持续时间;
        if (动画进度 < 0.0f) 动画进度 = 0.0f;
        动画进行中 = true;
    } else {
        动画进行中 = false;
    }
    float 动画系数 = 0.0f;
    if (动画进度 <= 0.5f) {
        动画系数 = 2.0f * 动画进度 * 动画进度;
    } else {
        动画系数 = 1.0f - 2.0f * (1.0f - 动画进度) * (1.0f - 动画进度);
    }
    ImVec2 当前窗口位置;
    当前窗口位置.x = 收缩位置.x + (目标位置.x - 收缩位置.x) * 动画系数;
    当前窗口位置.y = 收缩位置.y + (目标位置.y - 收缩位置.y) * 动画系数;
    
    ImVec2 当前窗口尺寸;
    当前窗口尺寸.x = 窗口最终尺寸.x * 动画系数;
    当前窗口尺寸.y = 窗口最终尺寸.y * 动画系数;
    
    if (悬浮窗 || 动画进行中) {
    ImVec4 originalButtonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
    ImVec4 transparentColor = originalButtonColor;
    transparentColor.w = 0.0f;
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 20.0f;
    
    static float targetWidth = 990.0f;
    static float currentWidth = 990.0f;
    static bool isAnimating = false;
    
    float newTargetWidth = 990.0f;
    if (show_ChildMenu1) {
        newTargetWidth = 1150.0f;
    } else if (show_ChildMenu2) {
        newTargetWidth = 1150.0f;
    } else {
        newTargetWidth = 990.0f;
    }
    
    if (fabs(newTargetWidth - targetWidth) > 1.0f) {
        targetWidth = newTargetWidth;
        isAnimating = true;
    }
    
    if (isAnimating) {
        Lerp(currentWidth, targetWidth, 0.1f);
        if (fabs(currentWidth - targetWidth) < 2.0f) {
            currentWidth = targetWidth;
            isAnimating = false;
        }
    }
    
        ImVec2 屏幕中心;
        if (Orientation == 0 || Orientation == 2) {
            屏幕中心 = ImVec2(displayInfo.width / 2.0f, displayInfo.height / 2.0f);
        } else {
            屏幕中心 = ImVec2(abs_ScreenX / 2.0f, abs_ScreenY / 2.0f);
        }
        
        ImVec2 窗口最终尺寸 = ImVec2(currentWidth, 730.0f);
        ImVec2 目标位置 = ImVec2(屏幕中心.x - 窗口最终尺寸.x / 2.0f, 屏幕中心.y - 窗口最终尺寸.y / 2.0f);

        ImVec2 收缩位置 = ImVec2(屏幕中心.x - 窗口最终尺寸.x / 2.0f, -窗口最终尺寸.y);
        ImVec2 当前窗口位置;
        当前窗口位置.x = 收缩位置.x + (目标位置.x - 收缩位置.x) * 动画系数;
        当前窗口位置.y = 收缩位置.y + (目标位置.y - 收缩位置.y) * 动画系数;

        ImVec2 当前窗口尺寸;
        当前窗口尺寸.x = 窗口最终尺寸.x * 动画系数;
        当前窗口尺寸.y = 窗口最终尺寸.y * 动画系数;
    ImVec2 minSize = ImVec2(当前窗口尺寸.x, 当前窗口尺寸.y);
    ImVec2 maxSize = ImVec2(当前窗口尺寸.x, FLT_MAX);
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
    ImGui::SetNextWindowPos(当前窗口位置, ImGuiCond_Always);
    
    style.WindowRounding = 20.0f * 动画系数;
    
    if(ImGui::Begin("币子", &悬浮窗, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
            bool 窗口打开状态 = (动画进度 > 0.3f);
            绘制.Pos = ImGui::GetWindowPos();
            ImVec2 窗口位置 = ImGui::GetWindowPos();
            ImVec2 窗口尺寸 = ImGui::GetWindowSize();
            if (背景图已加载 && 动画系数 > 0.3f) {
            绘制背景图();
            }
            if (窗口打开状态) {
                更新粒子系统(窗口位置, 窗口尺寸, 窗口打开状态);
                绘制粒子系统(窗口打开状态);
            }
            static int selectedMenu = 3;
            float targetMenuWidth = (currentWidth <= 990.0f) ? 200.0f : 248.0f;
            static float currentMenuWidth = 200.0f;
            if (fabs(currentMenuWidth - targetMenuWidth) > 0.1f) {
                Lerp(currentMenuWidth, targetMenuWidth, 0.12f);
            } else {
                currentMenuWidth = targetMenuWidth;
            }
            // if (ImGui::BeginChild("##Menu", ImVec2(currentMenuWidth - 20, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NavFlattened)) {
    // float availableHeight = ImGui::GetContentRegionAvail().y;
    // float targetGifHeight = 155.0f;
    // float currentGifHeight = targetGifHeight * 动画系数; 
    // float verticalSpacing = (availableHeight - 650 - currentGifHeight) * 0.5f;
    // if (verticalSpacing > 0) {
        // ImGui::Dummy(ImVec2(0, verticalSpacing));
    // }
 
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
    // ImGui::BeginChild("##GIFContainer", ImVec2(0, currentGifHeight + 10 * 动画系数), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    // ImGui::PopStyleVar();
    // ImGui::PopStyleColor();
    // ImGui::PopStyleVar();
    // float gifWidth = 160.0f * 动画系数; 
    // float horizontalSpacing = (ImGui::GetWindowWidth() - gifWidth) * 0.5f;
    // ImVec2 containerMin = ImGui::GetWindowPos();
    // ImVec2 containerMax = ImVec2(containerMin.x + ImGui::GetWindowWidth(), containerMin.y + ImGui::GetWindowHeight());
    // ImGui::SetCursorPosX(horizontalSpacing);
    // LoadAndRenderGifImage(fox_gif, fox_gif_len, ImVec2(gifWidth, currentGifHeight), 9999.0f);
    // ImGui::EndChild();
if (ImGui::BeginChild("##Menu", ImVec2(currentMenuWidth - 20, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NavFlattened)) {
    float availableHeight = ImGui::GetContentRegionAvail().y;
    float targetLogoHeight = 155.0f; // 保持原有高度
    float currentLogoHeight = targetLogoHeight * 动画系数; // 根据窗口动画进度调整高度
    float verticalSpacing = (availableHeight - 650 - currentLogoHeight) * 0.5f;
    if (verticalSpacing > 0) {
        ImGui::Dummy(ImVec2(0, verticalSpacing));
    }
 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
    ImGui::BeginChild("##LogoContainer", ImVec2(0, currentLogoHeight + 10 * 动画系数), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    
    // Logo加载逻辑（只执行一次）
    // static ImTextureID side_logo_texture = nullptr;
    // static bool logo_loaded = false;
    
    // if (!logo_loaded) {
        // // 检查手持图片map中是否存在快手Logo（key=60）
        // if (手持图片.find(60) != 手持图片.end()) {
            // side_logo_texture = 手持图片[60].textureId; // 使用DS纹理
            // logo_loaded = true;
            // printf("[INFO] 悬浮窗头像加载成功\n");
        // } else {
            // printf("[ERROR] 头像纹理未找到，key=60\n");
            // logo_loaded = true; // 避免重复尝试加载
        // }
    // }
        static ImTextureID side_logo_texture = nullptr;
    static bool logo_loaded = false;
    
    if (!logo_loaded) {
        // 检查手持图片map中是否存在快手Logo（key=60）
        if (手持图片.find(60) != 手持图片.end()) {
            side_logo_texture = 手持图片[60].textureId; // 使用DS纹理
            logo_loaded = true;
            printf("[INFO] 悬浮窗头像加载成功\n");
        } else {
            printf("[ERROR] 头像纹理未找到，key=60\n");
            logo_loaded = true; // 避免重复尝试加载
        }
    }

    float logoWidth = 160.0f * 动画系数; // 根据窗口动画进度调整宽度
    float horizontalSpacing = (ImGui::GetWindowWidth() - logoWidth) * 0.5f;
    ImVec2 containerMin = ImGui::GetWindowPos();
    ImVec2 containerMax = ImVec2(containerMin.x + ImGui::GetWindowWidth(), containerMin.y + ImGui::GetWindowHeight());
    
    // 设置光标位置居中
    ImGui::SetCursorPosX(horizontalSpacing);
    
    // 绘制Logo纹理（如果已加载）
    if (side_logo_texture != nullptr) {
        // 保持原有宽高比，计算合适的高度
        float aspectRatio = 1.0f; // 根据实际图片宽高比调整
        float adjustedHeight = currentLogoHeight;
        float adjustedWidth = logoWidth;
        
        // 如果需要保持宽高比，可以这样计算：
        // adjustedHeight = logoWidth / aspectRatio;
        // if (adjustedHeight > currentLogoHeight) {
        //     adjustedHeight = currentLogoHeight;
        //     adjustedWidth = adjustedHeight * aspectRatio;
        // }
        
        ImGui::SetCursorPosY((currentLogoHeight - adjustedHeight) * 0.5f);
        ImGui::Image(side_logo_texture, ImVec2(adjustedWidth, adjustedHeight));
    } else {
        // 如果纹理未加载，显示一个占位矩形
        ImGui::SetCursorPosY((currentLogoHeight - logoWidth) * 0.5f);
        ImGui::Dummy(ImVec2(logoWidth, logoWidth));
    }
    
    ImGui::EndChild();
    ImGui::Separator();  
    ImGui::Separator();  
    vector<string> tabNames = {"绘制", "触摸", "设置"};
    static int lastTabIndex = -1;
    float targetTabBarWidth = ((currentMenuWidth - 20) <= 180.0f) ? 160.0f : 200.0f;
    float currentTabBarWidth = targetTabBarWidth * 动画系数; 
    float tabBarHeight = 165.0f * 动画系数;
    VerticalTabBar(tabNames, tap页[1], currentTabBarWidth, tabBarHeight, 20.0f * 动画系数, "main_menu");
    int currentTabIndex = (int)tap页[1];
    if (lastTabIndex != currentTabIndex) {
        lastTabIndex = currentTabIndex;
        show_ChildMenu1 = false;
        show_ChildMenu2 = false;
        show_ChildMenu3 = false;
        show_ChildMenu4 = false;        
        switch (currentTabIndex) {
            case 0:
                show_ChildMenu1 = true;
                break;
            case 1:
                show_ChildMenu2 = true;
                break;
            case 2:
                show_ChildMenu3 = true;
                break;
            default:
                show_ChildMenu1 = true;
                break;
        }
    }
    ImGui::EndChild();
}
ImGui::SameLine();

            float contentWidth = ImGui::GetContentRegionAvail().x;
            static float targetContentWidth = contentWidth;
            static float currentContentWidth = contentWidth;
            if (fabs(currentContentWidth - targetContentWidth) > 0.1f) {
                Lerp(currentContentWidth, targetContentWidth, 0.08f);
            } else {
                currentContentWidth = targetContentWidth;
            }
            targetContentWidth = contentWidth;
            
            if (show_ChildMenu1) {
    ImGui::BeginChild("##绘制", ImVec2(-1, -1), false, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
    float targetSubMenuWidth = (currentWidth <= 990.0f) ? 180.0f : 200.0f;
    static float currentSubMenuWidth = 180.0f;
    if (fabs(currentSubMenuWidth - targetSubMenuWidth) > 0.1f) {
        Lerp(currentSubMenuWidth, targetSubMenuWidth, 0.1f);
    } else {
        currentSubMenuWidth = targetSubMenuWidth;
    }
    ImGui::BeginChild("##绘制左侧菜单", ImVec2(currentSubMenuWidth, -1), true, ImGuiWindowFlags_NoScrollbar);
    vector<string> drawTabNames = {"基础设置", "地图雷达", "粗细调节", "物资绘制"}; 
    static float selectedDrawTab = 0;
    static int lastDrawTabIndex = -1;
    static std::map<int, float> tabSelectionMap; 
    int currentMainTabIndex = (int)tap页[1];
    if (tabSelectionMap.find(currentMainTabIndex) == tabSelectionMap.end()) {
        tabSelectionMap[currentMainTabIndex] = 0; 
    }
    selectedDrawTab = tabSelectionMap[currentMainTabIndex];
    float targetVerticalTabWidth = (currentSubMenuWidth <= 180.0f) ? 160.0f : 180.0f;
    static float currentVerticalTabWidth = 160.0f;
    if (fabs(currentVerticalTabWidth - targetVerticalTabWidth) > 0.1f) {
        Lerp(currentVerticalTabWidth, targetVerticalTabWidth, 0.1f);
    } else {
        currentVerticalTabWidth = targetVerticalTabWidth;
    }
    VerticalTabBar(drawTabNames, selectedDrawTab, currentVerticalTabWidth, 165, 20.0f, "draw_submenu");
    int currentDrawTabIndex = (int)selectedDrawTab;
    if (lastDrawTabIndex != currentDrawTabIndex) {
        lastDrawTabIndex = currentDrawTabIndex;
        tabSelectionMap[currentMainTabIndex] = selectedDrawTab; 
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("##绘制内容区域", ImVec2(-1, -1), true);
    int activeSubMenuIndex = (int)tabSelectionMap[currentMainTabIndex];
    switch (activeSubMenuIndex) {
        case 0: { 
            static bool 全部开启 = false;
            if (全部开启) {
                绘制.按钮.人数 = true;
                绘制.按钮.方框 = true;
                绘制.按钮.血量 = true;
                绘制.按钮.距离 = true;
                绘制.按钮.射线 = true;
                绘制.按钮.名字 = true;
                绘制.按钮.骨骼 = true;
                绘制.按钮.手持 = true;
                绘制.按钮.车辆 = true;
                绘制.按钮.手雷预警 = true;
                绘制.按钮.盒子 = true;
                绘制.按钮.背敌预警 = true;
            }
                float buttonWidth = contentWidth;
    const char* button_text = is_connected ? "退出程序" : "链接数据";
    if (ImGui::Button(button_text, ImVec2(buttonWidth, 50))) {
        if (!is_connected) {
            绘制.初始化绘制("com.tencent.ig", abs_ScreenX, abs_ScreenY);
            绘制.按钮.绘制 = true;
            is_connected = true;
        } else {
        system("pkill -f 币子手持美化.sh");//不退出美化进程会卡死
            exit(1);
        }
    }    

                      if (M_CircularCheckbox("初始化漏打[开关两次]", &绘制.按钮.手动开启)) {
                            模型();
                            绘制.保存配置();
                        }
                        ImGui::SameLine();
                        if (M_CircularCheckbox("绘制模型", &绘制模型)) 
                        绘制.保存配置();

            if (M_CircularCheckbox("绘制人数", &绘制.按钮.人数))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("绘制方框", &绘制.按钮.方框))
                绘制.保存配置();
            if (M_CircularCheckbox("绘制血量", &绘制.按钮.血量))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("绘制手持", &绘制.按钮.手持))
                绘制.保存配置();
            if (M_CircularCheckbox("绘制背敌", &绘制.按钮.背敌预警))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("绘制距离", &绘制.按钮.距离))
                绘制.保存配置();
            if (M_CircularCheckbox("绘制射线", &绘制.按钮.射线))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("绘制名字", &绘制.按钮.名字))
                绘制.保存配置();
            if (M_CircularCheckbox("绘制骨骼", &绘制.按钮.骨骼))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("绘制车辆", &绘制.按钮.车辆))
                绘制.保存配置();
            if (M_CircularCheckbox("载具油量", &绘制.按钮.载具油量))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("手雷预警", &绘制.按钮.手雷预警))
                绘制.保存配置();
            if (M_CircularCheckbox("忽略人机", &绘制.按钮.忽略人机))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("自救救援倒计", &绘制.按钮.自救救人))
                绘制.保存配置();
                
            if (M_CircularCheckbox("闪框解密", &绘制.按钮.防闪框))
                绘制.保存配置();

            ImGui::BulletText("绘图调节");
            if (ImGui::Combo("手持绘图样式", &绘制.按钮.手持绘图, "手持武器文字\0手持武器图片\0"))
                绘制.保存配置();
                
                if (ImGui::SliderInt("观透自身队伍", &绘制.按钮.观透队伍, 0, 100)) {
    绘制.保存配置();
}
if (ImGui::SliderInt("地铁观透自身", &绘制.按钮.观透队伍, 0, 10)) {
    绘制.保存配置();
}
            M_CircularCheckbox1("广角开启", &广角设置);
    M_CustomSliderFloatvv("广角大小", &绘制.按钮.第三人称, 90, 130, "%.2f" );


            break;
        }
        case 1: { 
        if (ImGui::Button("币子手持美化[请下载资源]", ImVec2(-1, 60))) {//@QiuZhi886 @bizinb
    if (!美化中.load()) {
        美化中.store(true);
        
        // 使用 [&] 捕获所有外部变量引用
        美化线程 = std::thread([&]{
            美化();
            美化中.store(false);
        });
        
        美化线程.detach();
    } else {
        ImGui::Text("美化正在进行中...");
    }
}
        if (M_CircularCheckbox1("开关", &绘制.按钮.雷达))
        绘制.保存配置();
        // ImGui::SameLine();
        if (M_CircularCheckbox3("显圆点", &绘制.按钮.显示敌人圆点))
        绘制.保存配置();
        ImGui::SameLine();
        if (M_CircularCheckbox3("显距离", &绘制.按钮.显示距离))
        绘制.保存配置();
        ImGui::SameLine();
        if (M_CircularCheckbox3("显视角", &绘制.按钮.显示敌人视角朝向))
        绘制.保存配置();
        
        if (M_CustomSliderFloatvv("X", &绘制.按钮.雷达X, 0.0f, 3000.0f, "%.1f", 1))
        绘制.保存配置();
        if (M_CustomSliderFloatvv("Y", &绘制.按钮.雷达Y, 0.0f, 2120.0f, "%.1f", 2))
        绘制.保存配置();
        if (M_CustomSliderFloatvv("雷达大小", &绘制.按钮.雷达大小, 0.30f, 3.0f, "%.1f", 1))
        绘制.保存配置();
        if (M_CustomSliderFloatvv("圆点大小", &绘制.按钮.圆点大小,1.0f, 10.0f, "%.1f", 2))
        绘制.保存配置();
        if (M_CustomSliderFloatvv("雷达视距", &绘制.按钮.雷达视距, 50.0f, 300.0f, "%.1f", 3))
        绘制.保存配置();
            break;
        }
case 2: { 
    static int ColorSettings = 1;
    ImGui::Text("颜色配置");
    ImGui::Combo("当前配置", &ColorSettings, "人机\0真人\0");
    
    if (ColorSettings == 1) {
        // 真人颜色配置
        if (币子の调色器("方框颜色", 绘制.Colorset[0].方框颜色, 1, true)) 
            绘制.保存配置();
        if (币子の调色器("射线颜色", 绘制.Colorset[0].射线颜色, 1, true)) 
            绘制.保存配置();
        if (币子の调色器("骨骼颜色", 绘制.Colorset[0].骨骼颜色, 1, true)) 
            绘制.保存配置();
        if (币子の调色器("血量颜色", 绘制.Colorset[0].血量颜色, 1, true)) 
            绘制.保存配置();
        if (币子の调色器("距离颜色", 绘制.Colorset[0].距离颜色, 1, true)) 
            绘制.保存配置();
        if (币子の调色器("名称颜色", 绘制.Colorset[0].名称颜色, 1, true)) 
            绘制.保存配置();
    } else {
        // 人机颜色配置
        if (币子の调色器("方框颜色", 绘制.Colorset[1].方框颜色, 0, true)) 
            绘制.保存配置();
        if (币子の调色器("射线颜色", 绘制.Colorset[1].射线颜色, 0, true)) 
            绘制.保存配置();
        if (币子の调色器("骨骼颜色", 绘制.Colorset[1].骨骼颜色, 0, true)) 
            绘制.保存配置();
        if (币子の调色器("血量颜色", 绘制.Colorset[1].血量颜色, 0, true)) 
            绘制.保存配置();
        if (币子の调色器("距离颜色", 绘制.Colorset[1].距离颜色, 0, true)) 
            绘制.保存配置();
        if (币子の调色器("名称颜色", 绘制.Colorset[1].名称颜色, 0, true)) 
            绘制.保存配置();
    }
    
    // 物资颜色配置
    if (币子の调色器("物资颜色", 绘制.物资颜色, 1, true))
        绘制.保存配置();
    break;
}
        case 3: { 
            ImGui::Text("  · 特殊物品"); 
            if (M_CircularCheckbox("空投", &绘制.按钮.绘制空投))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("盒子文字", &绘制.按钮.盒子文字))
                绘制.保存配置();          
            if (M_CircularCheckbox("盒子图片", &绘制.按钮.盒子图片))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("金插", &绘制.按钮.绘制金插))
                绘制.保存配置();
            if (M_CircularCheckbox("投掷物品", &绘制.按钮.投掷物品))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("召回信号枪", &绘制.按钮.绘制信号枪))
                绘制.保存配置();
            ImGui::Separator();
            ImGui::Text("  · 枪械绘制");           
            if (M_CircularCheckbox("显示步枪", &绘制.按钮.显示步枪))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("冲锋枪械", &绘制.按钮.冲锋枪械))
                绘制.保存配置();
            if (M_CircularCheckbox("狙击枪械", &绘制.按钮.狙击枪械))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("散弹枪械", &绘制.按钮.散弹枪械))
                绘制.保存配置();
            if (M_CircularCheckbox("爆炸猎弓", &绘制.按钮.爆炸猎弓))         
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("手枪枪械", &绘制.按钮.手枪枪械))         
                绘制.保存配置();            
            ImGui::Separator();  
            ImGui::Text("  · 子弹绘制"); 
            if (M_CircularCheckbox("5.56", &绘制.按钮.显示556子弹))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("7.62", &绘制.按钮.显示762子弹))
                绘制.保存配置(); 
            if (M_CircularCheckbox("9毫米", &绘制.按钮.显示9mm子弹))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox(".45口径", &绘制.按钮.显示45mm子弹))
                绘制.保存配置();
            if (M_CircularCheckbox("霰弹", &绘制.按钮.显示霰弹))          
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("信号弹", &绘制.按钮.显示信号弹))
                绘制.保存配置();
            if (M_CircularCheckbox("箭矢", &绘制.按钮.显示箭矢))
                绘制.保存配置();
            ImGui::Separator();
            ImGui::Text("  · 头甲绘制"); 
            if (M_CircularCheckbox("三级头", &绘制.按钮.显示三级头))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("三级甲", &绘制.按钮.显示三级甲))
                绘制.保存配置();
            if (M_CircularCheckbox("三级包", &绘制.按钮.显示三级包))
                绘制.保存配置();
            ImGui::Separator(); 
            ImGui::Text("  · 药品绘制"); 
            if (M_CircularCheckbox("医疗箱", &绘制.按钮.显示医疗箱))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("急救包", &绘制.按钮.显示急救包))
                绘制.保存配置();
            if (M_CircularCheckbox("绷带", &绘制.按钮.显示绷带))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("能量饮料", &绘制.按钮.显示可乐))
                绘制.保存配置();
            if (M_CircularCheckbox("肾上腺素", &绘制.按钮.显示肾上腺素))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("止痛药", &绘制.按钮.显示止痛药))
                绘制.保存配置();
            ImGui::Separator();
            ImGui::Text("  · 配件绘制"); 
            if (M_CircularCheckbox("子弹袋", &绘制.按钮.显示子弹袋))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("箭袋", &绘制.按钮.显示箭袋))
                绘制.保存配置();
            if (M_CircularCheckbox("激光瞄准器", &绘制.按钮.显示激光瞄准器))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("轻型握把", &绘制.按钮.显示轻型握把))
                绘制.保存配置();
            if (M_CircularCheckbox("半截握把", &绘制.按钮.显示半截握把))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("UZI枪托", &绘制.按钮.显示UZI枪托))
                绘制.保存配置();
            if (M_CircularCheckbox("狙击枪托", &绘制.按钮.显示狙击枪托))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("步枪枪托", &绘制.按钮.显示步枪枪托))
                绘制.保存配置();
            if (M_CircularCheckbox("狙击枪补偿器", &绘制.按钮.显示狙击枪补偿器))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("狙击枪消焰器", &绘制.按钮.显示狙击枪消焰器))
                绘制.保存配置();
            if (M_CircularCheckbox("狙击枪消音器", &绘制.按钮.显示狙击枪消音器))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("步枪消音器", &绘制.按钮.显示步枪消音器))
                绘制.保存配置();
            if (M_CircularCheckbox("步枪补偿器", &绘制.按钮.显示步枪补偿器))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("步枪消焰器", &绘制.按钮.显示步枪消焰器))
                绘制.保存配置();
            if (M_CircularCheckbox("冲锋枪消音器", &绘制.按钮.显示冲锋枪消音器))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("冲锋枪消焰器", &绘制.按钮.显示冲锋枪消焰器))
                绘制.保存配置();
            if (M_CircularCheckbox("拇指握把", &绘制.按钮.显示拇指握把))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("垂直握把", &绘制.按钮.显示垂直握把))
                绘制.保存配置();
            if (M_CircularCheckbox("直角握把", &绘制.按钮.显示直角握把))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("撞火枪托", &绘制.按钮.显示撞火枪托))
                绘制.保存配置();
            if (M_CircularCheckbox("霰弹快速", &绘制.按钮.显示霰弹快速))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("鸭嘴枪口", &绘制.按钮.显示鸭嘴枪口))
                绘制.保存配置();
            if (M_CircularCheckbox("霰弹收束", &绘制.按钮.显示霰弹收束))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("显示扩容", &绘制.按钮.显示扩容))
                绘制.保存配置();
            ImGui::Separator();
            ImGui::Text("  · 地铁绘制");
            if (M_CircularCheckbox("宝箱绘制", &绘制.按钮.绘制宝箱))
                绘制.保存配置();
            if (M_CircularCheckbox("武器箱绘制", &绘制.按钮.绘制武器箱))
                绘制.保存配置();
            if (M_CircularCheckbox("超级物资箱", &绘制.按钮.超级物资箱))
                绘制.保存配置();
            if (M_CircularCheckbox("隐藏已开启宝箱、药箱、武器箱", &绘制.按钮.隐藏已开启))
                绘制.保存配置();
            break;
        }
    }
    ImGui::EndChild(); 
    ImGui::EndChild(); 
}
 else if (show_ChildMenu2) {
    ImGui::BeginChild("##自瞄", ImVec2(-1, -1), false, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::BeginChild("##自瞄左侧菜单", ImVec2(200, -1), true, ImGuiWindowFlags_NoScrollbar);
    vector<string> aimTabNames = {"触摸自瞄", "枪械压枪", "基础调节", "预判调节"};
    static float selectedAimTab = 0;
    static int lastAimTabIndex = -1;
    static std::map<int, float> tabSelectionMap; 
    int currentMainTabIndex = (int)tap页[1];
    if (tabSelectionMap.find(currentMainTabIndex) == tabSelectionMap.end()) {
        tabSelectionMap[currentMainTabIndex] = 0; 
    }
    selectedAimTab = tabSelectionMap[currentMainTabIndex];
    VerticalTabBar(aimTabNames, selectedAimTab, 180, 165, 20.0f, "aim_submenu");
    int currentAimTabIndex = (int)selectedAimTab;
    if (lastAimTabIndex != currentAimTabIndex) {
        lastAimTabIndex = currentAimTabIndex;
        tabSelectionMap[currentMainTabIndex] = selectedAimTab; 
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("##自瞄内容区域", ImVec2(-1, -1), true);
    int activeSubMenuIndex = (int)tabSelectionMap[currentMainTabIndex];
    switch (activeSubMenuIndex) {
        case 0: { 
            if (M_CircularCheckbox("打开自瞄", &绘制.自瞄.初始化))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("快捷自瞄", &绘制.自瞄.自瞄控件))
                绘制.保存配置();
            if (M_CircularCheckbox("触摸位置", &绘制.自瞄.触摸位置))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("动态自瞄", &绘制.自瞄.动态自瞄))
                绘制.保存配置();
            if (M_CircularCheckbox("准星射线", &绘制.自瞄.准星射线))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("倒地不瞄", &绘制.自瞄.倒地不瞄))
                绘制.保存配置();
            if (M_CircularCheckbox("人机不瞄", &绘制.自瞄.人机不瞄))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("隐藏自瞄", &绘制.自瞄.隐藏自瞄圈))
                绘制.保存配置();
            if (M_CircularCheckbox("随机触摸", &绘制.自瞄.随机触摸点))
                绘制.保存配置();
                ImGui::SameLine();
            if (M_CircularCheckbox("烟雾不瞄", &绘制.自瞄.烟雾不触发自瞄))
                绘制.保存配置();                     
            ImGui::Text("  · 喷子/狙击自瞄"); 
            if (M_CircularCheckbox("开镜狙击自瞄", &绘制.自瞄.狙击自瞄))
                绘制.保存配置();   
            ImGui::SameLine();  
            if (M_CircularCheckbox("喷子自瞄", &绘制.自瞄.喷子自瞄))
                绘制.保存配置(); 
            ImGui::Text("  · 自瞄模式");   
            if (M_CircularCheckbox("框内自瞄", &绘制.自瞄.框内自瞄))
                绘制.保存配置();
            ImGui::SameLine();
            if (M_CircularCheckbox("软锁自瞄", &绘制.自瞄.软锁自瞄))
                绘制.保存配置();
            if (M_CircularCheckbox("掉血自瞄", &绘制.自瞄.掉血自瞄))
                绘制.保存配置();  
                ImGui::SameLine();
            if (M_CircularCheckbox("近战开镜不瞄", &绘制.自瞄.近战限制开关))
                绘制.保存配置();  
            if (绘制.自瞄.软锁自瞄) {              
                绘制.mk20 = 0.25f;
            }
            if (!绘制.自瞄.软锁自瞄) {
                绘制.mk20 = 绘制.备份.mk20压枪;
            }
            if (ImGui::Combo("自瞄条件", &绘制.自瞄.自瞄条件, "开火\0开镜\0开火||开镜\0"))
                绘制.保存配置();
            if (ImGui::Combo("喷子自瞄触发条件", &绘制.自瞄.喷子自瞄条件, "持续锁定\0开镜锁定\0"))
                绘制.保存配置();
            if (ImGui::Combo("充电口方向", &绘制.自瞄.充电口方向, "右边\0左边\0"))
                绘制.保存配置();
            if (ImGui::Combo("自瞄优先", &绘制.自瞄.瞄准优先, "准星\0距离\0"))
                绘制.保存配置();
            if (ImGui::Combo("瞄准部位", &绘制.自瞄.瞄准部位, "头部\0胸部\0臀部\0指哪打哪\0"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("触摸范围大小", &绘制.自瞄.触摸范围, 5, 600, "%.0f"))
                绘制.保存配置();
            break;
        }
        case 1: { 
            ImGui::Text("  · 步枪压枪");
            if (M_CustomSliderFloatvv("M416", &绘制.自瞄.m416, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("SCAR-L", &绘制.自瞄.scar_l, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("AUG", &绘制.自瞄.aug, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("FAMAS", &绘制.自瞄.famas, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("G36C", &绘制.自瞄.g36c, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("M249", &绘制.自瞄.m249, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("AKM", &绘制.自瞄.akm, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("M762", &绘制.自瞄.m762, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("蜜獾", &绘制.自瞄.蜜獾, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("PKM", &绘制.自瞄.pkm, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("MG3", &绘制.自瞄.mg3, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("MG-36", &绘制.自瞄.mg_36, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            ImGui::Text("  · 冲锋枪压枪");
            if (M_CustomSliderFloatvv("P90", &绘制.自瞄.p90, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("UZIi", &绘制.自瞄.uzi, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("UMP45", &绘制.自瞄.ump45, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("VECTOR", &绘制.自瞄.vector, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("汤姆逊", &绘制.自瞄.汤姆逊, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("野牛", &绘制.自瞄.野牛, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            ImGui::Text("  · 射手步枪");
            if (M_CustomSliderFloatvv("MINI14", &绘制.自瞄.mini14, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("SKS", &绘制.自瞄.sks, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("M417", &绘制.自瞄.m417, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("MK20-H", &绘制.自瞄.mk20_h, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("MK12", &绘制.自瞄.mk12, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("VSS", &绘制.自瞄.vss, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            ImGui::Text("  · 握把压枪");
            if(M_CustomSliderFloatvv("轻型握把", &绘制.自瞄.轻型压枪力度, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("垂直握把", &绘制.自瞄.垂直压枪力度, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("拇指握把", &绘制.自瞄.拇指压枪力度, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("直角握把", &绘制.自瞄.直角压枪力度, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            break;
        }
        case 2: { 
            ImGui::Text("  · 自瞄调节");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::BeginItemTooltip()) {
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(
                    "参数调节详细\n"
                    "1.自瞄速度 越小越快\n"
                    "2.压枪力度 越大越压\n");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
            if (M_CustomSliderFloatvv("自瞄范围", &绘制.自瞄.自瞄范围, 10, 300, "%.0f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("压枪力度", &绘制.自瞄.压枪力度, 0.1, 15, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("平滑速度", &绘制.自瞄.自瞄速度, 0.f, 80.f, "%.0f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("掉血自瞄概率", &绘制.自瞄.掉血自瞄数率, 5, 25, "%.0f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("近战开镜不瞄距离", &绘制.自瞄.近战限制距离, 2, 15, "%.0f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("预判速度", &绘制.自瞄.预判力度, 0.0, 2.0, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("扫车预判", &绘制.预判度.扫车, 0.0, 4.f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("腰射自瞄距离", &绘制.自瞄.腰射距离限制, 0.0, 300, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("开镜自瞄距离", &绘制.自瞄.自瞄距离限制, 0.0, 300, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("喷子距离限制", &绘制.自瞄.喷子距离限制, 0, 50, "%.0f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("趴下压枪力度", &绘制.自瞄.趴下位置调节, 0, 5, "%.2f"))
                绘制.保存配置();
                ImGui::Text("  · 距离压枪"); 
            if(M_CustomSliderFloatvv("50-100M", &绘制.自瞄.压枪系数_50_100, 0.1f, 2.0f, "%.2f")) {
                绘制.保存配置();
            }
            if(M_CustomSliderFloatvv("100-150M", &绘制.自瞄.压枪系数_100_150, 0.1f, 2.0f, "%.2f")) {
                绘制.保存配置();
            }
            if(M_CustomSliderFloatvv("150-200M", &绘制.自瞄.压枪系数_150_200, 0.1f, 2.0f, "%.2f")) {
                绘制.保存配置();
            }
            if(M_CustomSliderFloatvv("200-250M", &绘制.自瞄.压枪系数_200_250, 0.1f, 3.0f, "%.2f")) {
                绘制.保存配置();
            }
            if(M_CustomSliderFloatvv("250-300M", &绘制.自瞄.压枪系数_250_300, 0.1f, 3.0f, "%.2f")) {
                绘制.保存配置();
            }
                    if(绘制.自瞄模式 ==1){
        
                    ImGui::Text("  · 陀螺仪自瞄调节");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::BeginItemTooltip()) {
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(
                    "参数调节详细\n"
                    "力度越大 越锁\n");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
            M_CircularCheckbox("无目标压枪自瞄", &绘制.自瞄.无目标压枪开关);
if (M_CustomSliderFloatvv("压枪陀螺仪", &绘制.自瞄.基础下压力度, 0, 20, "%.1f")) {
    绘制.保存配置();
}

if (M_CustomSliderFloatvv("不开镜陀螺仪灵敏度", &绘制.按钮.不开镜灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}
if (M_CustomSliderFloatvv("红点陀螺仪灵敏度", &绘制.按钮.机瞄红点灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}
if (M_CustomSliderFloatvv("二倍镜陀螺仪灵敏度", &绘制.按钮.二倍灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}
if (M_CustomSliderFloatvv("三倍镜陀螺仪灵敏度", &绘制.按钮.三倍灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}
if (M_CustomSliderFloatvv("四倍镜陀螺仪灵敏度", &绘制.按钮.四倍灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}
if (M_CustomSliderFloatvv("六倍镜陀螺仪灵敏度", &绘制.按钮.六倍灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}
if (M_CustomSliderFloatvv("八倍镜陀螺仪灵敏度", &绘制.按钮.八倍灵敏度, 0, 600, "%.0f"))
{
                   绘制.保存配置();
}

        
        }

            break;
        }
        case 3: { 
            ImGui::Text("  · 预判调节"); 
            if (M_CustomSliderFloatvv("M416", &绘制.预判度.m416, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("scar_l", &绘制.预判度.scar_l, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("aug", &绘制.预判度.aug, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("famas", &绘制.预判度.famas, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("g36c", &绘制.预判度.g36c, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("m249", &绘制.预判度.m249, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("akm", &绘制.预判度.akm, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("m762", &绘制.预判度.m762, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("蜜獾", &绘制.预判度.蜜獾, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("pkm", &绘制.预判度.pkm, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("mg3", &绘制.预判度.mg3, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("mg-36", &绘制.预判度.mg_36, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("p90", &绘制.预判度.p90, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("uzi", &绘制.预判度.uzi, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("ump45", &绘制.预判度.ump45, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("vector", &绘制.预判度.vector, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("汤姆逊", &绘制.预判度.汤姆逊, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            if (M_CustomSliderFloatvv("野牛", &绘制.预判度.野牛, 0.00f, 5.0f, "%.2f"))
                绘制.保存配置();
            break;
        }
    }
    ImGui::EndChild(); 
    ImGui::EndChild(); 
} else if (show_ChildMenu3) {
    float contentWidth = ImGui::GetContentRegionAvail().x;
    ImGui::BeginChild("##设置", ImVec2(contentWidth, -1), true, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
    float buttonWidth = contentWidth;
    const char* button_text = is_connected ? "退出程序" : "链接数据";
    if (ImGui::Button(button_text, ImVec2(buttonWidth, 60))) {
        if (!is_connected) {
            绘制.初始化绘制("com.tencent.tmgp.pubgmhd", abs_ScreenX, abs_ScreenY);
            绘制.按钮.绘制 = true;
            is_connected = true;
        } else {
        system("pkill -f 币子手持美化.sh");//不退出美化进程会卡死
            exit(1);
        }
    }    
    float saveDeleteWidth = (buttonWidth - ImGui::GetStyle().ItemSpacing.x) / 2;
    if (ImGui::Button("保存参数", ImVec2(saveDeleteWidth, 50))) {
        绘制.保存配置();
    }
    ImGui::SameLine();
    if (ImGui::Button("删除参数", ImVec2(saveDeleteWidth, 50))) {
                     system("rm -f /data/币子内核配置");
                 system("rm -f /data/币子按钮配置");
    }        
    M_CircularCheckbox1("开发者模式", &绘制.按钮.Debug);
    
        ImGui::Text("帧率调节");
        ImGui::SameLine();
    static float 帧率选项 = 90.0f;
    vector<string> 帧率选项名称 = {"60帧", "90帧", "120帧", "144帧"};
    fanhua(帧率选项名称, 帧率选项, 0, 120, 50, 10);
    if (帧率选项 == 0) {
        绘制.按钮.帧率选项 = 60;
    } else if (帧率选项 == 1) {
        绘制.按钮.帧率选项 = 90;
    } else if (帧率选项 == 2) {
        绘制.按钮.帧率选项 = 120;
    } else if (帧率选项 == 3) {
        绘制.按钮.帧率选项 = 144;
    }
   
    ImGui::Text("绘图样式       "); 
    ImGui::SameLine();
    ImGui::Combo("##血条样式", &绘制.按钮.血条样式, "默认\0文字\0圆圈\0胶囊\0");
    ImGui::Text("背景粒子样式"); 
    ImGui::SameLine();
    const char* particle_styles[] = {"雪花纷飞", "彩花浮江", "落叶纷飞(自定义: 飘落.png)", "空空如也"};
    const char* particle_combo_preview = particle_styles[currentParticleStyle];
    if (ImGui::BeginCombo("##粒子样式", particle_combo_preview)) {
        for (int i = 0; i < IM_ARRAYSIZE(particle_styles); i++) {
            bool is_selected = (currentParticleStyle == i);
            if (ImGui::Selectable(particle_styles[i], is_selected)) {
                currentParticleStyle = i;
                切换粒子样式(currentParticleStyle);
                绘制.保存配置();
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (M_CustomSliderFloatvv("触摸采样", &绘制.自瞄.触摸采样率, 200, 1200, "%.0f")) {
        绘制.保存配置();
    }
    if (M_CustomSliderFloatvv("骨骼显示距离", &绘制.骨骼距离限制, 0.0f, 300.0f, "%.0f", 1)) {
        绘制.保存配置();
    }
    M_CustomSliderFloatvv("方框粗细", &绘制.按钮.方框粗细, 0.1, 5, "%.1f");
    M_CustomSliderFloatvv("射线粗细", &绘制.按钮.射线粗细, 0.1, 5, "%.1f");
    M_CustomSliderFloatvv("骨骼粗细", &绘制.按钮.骨骼粗细, 0.1, 5, "%.1f");
    
    static bool isRecording = false;
    static bool isTimerRunning = false;
    static float timerStartTime = 0.0f;
    static float 录屏时长 = 30.0f;
    static bool 录屏 = false;
    static float lp_beg = 0.0f, lp_end = 0.0f;
    static int 录屏比特率 = 30000000;
    static std::thread recordingThread;
    static bool threadRunning = false;
    lp_beg += (lp_end - lp_beg) * (0.2f * speed);
    const char* 录制按钮文字 = isRecording ? "结束录屏" : "开启录制";
    if (ImGui::Button(录制按钮文字, ImVec2(buttonWidth, 75))) {
        if (!isRecording) {
            lp_end = 400;
            isTimerRunning = true;
            timerStartTime = static_cast<float>(ImGui::GetTime());
            isRecording = true;
            录屏 = true;
            string beijingTime = getBeijingTimeString();
            int randomNum = rand() % 10000;
            string fileName = "/sdcard/录制_" + beijingTime + "_" + std::to_string(randomNum) + ".mp4";
            string command = "screenrecord --bit-rate " + std::to_string(录屏比特率) + " --time-limit " + std::to_string((int)录屏时长) + " " + fileName;
            if (threadRunning && recordingThread.joinable()) {
                recordingThread.detach(); 
            }
            threadRunning = true;
            recordingThread = std::thread([command, fileName]() {
                int result = system(command.c_str());
                if (result == 0) {
                    printf("录屏完成: %s\n", fileName.c_str());
                } else {
                    printf("录屏失败，错误码: %d\n", result);
                }
                isRecording = false;
                录屏 = false;
                isTimerRunning = false;
                threadRunning = false;
            });
        } else {
            lp_end = 0;
            int killResult = system("pkill -2 screenrecord");
            if (killResult != 0) {
                system("pkill -INT screenrecord");
            }
            isRecording = false;
            录屏 = false;
            isTimerRunning = false;
            if (threadRunning && recordingThread.joinable()) {
                recordingThread.join();
                threadRunning = false;
            }
        }
    }
    static bool 正在截屏 = false;
    if (ImGui::Button("截屏", ImVec2(buttonWidth, 75))) {
        if (!正在截屏) {
            正在截屏 = true;
            string beijingTime = getBeijingTimeString();
            int randomNum = rand() % 10000;
            string fileName = "/sdcard/截图_" + beijingTime + "_" + std::to_string(randomNum) + ".png";
            string command = "screencap -p " + fileName;
            std::thread([command, fileName]() {
                int result = system(command.c_str());
                if (result == 0) {
                    printf("截屏成功: %s\n", fileName.c_str());
                } else {
                    printf("截屏失败，错误码: %d\n", result);
                }
                正在截屏 = false;
            }).detach();
        }
    }
    if (isTimerRunning && (ImGui::GetTime() - timerStartTime >= 录屏时长)) {
        lp_end = 0;
        isTimerRunning = false;
        system("pkill -2 screenrecord");
        录屏 = false;
        isRecording = false;
        if (threadRunning && recordingThread.joinable()) {
            recordingThread.join();
            threadRunning = false;
        }
    }
    static float 自定义比特率值 = 30.0f;    
    if (M_CustomSliderFloatvv("录制比特率", &自定义比特率值, 1.0f, 50.0f, "%.1f Mbps")) {
        录屏比特率 = (int)(自定义比特率值 * 1000000);
        绘制.保存配置();
    }    
    if (M_CustomSliderFloatvv("录制时长", &录屏时长, 1, 180, "%.0f 秒")) {
        录屏时长 = ImClamp(录屏时长, 1.0f, 180.0f);
        绘制.保存配置();
    }    
    if (ImGui::CollapsingHeader("头像设置")) {
    // 当前头像样式显示
    const char* 样式名称[] = {"微信头像", "自定义头像", "默认头像"};
    int 当前样式 = 0;
    
    for (int i = 0; i < 3; i++) {
        char 文件路径[100];
        sprintf(文件路径, "/storage/emulated/0/币子/头像样式%d", i);
        if (access(文件路径, F_OK) == 0) {
            当前样式 = i;
            break;
        }
    }
    
    ImGui::Text("当前: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0, 1, 0, 1), 样式名称[当前样式]);
    
    ImGui::Separator();
    
    // 切换按钮
    for (int i = 0; i < 3; i++) {
        char 按钮标签[50];
        sprintf(按钮标签, "切换到%s", 样式名称[i]);
        
        if (ImGui::Button(按钮标签, ImVec2(-1, 40)) && 当前样式 != i) {
            // 清理所有样式文件
            for (int j = 0; j < 3; j++) {
                char 文件路径[100];
                sprintf(文件路径, "/storage/emulated/0/币子/头像样式%d", j);
                remove(文件路径);
            }
            
            // 创建新样式文件
            char 新文件路径[100];
            sprintf(新文件路径, "/storage/emulated/0/币子/头像样式%d", i);
            FILE* file = fopen(新文件路径, "w");
            if (file) fclose(file);
            
            // 重新加载头像
            加载头像();
        }
    }
}

    ImGui::Spacing(); 
    ImGui::Spacing(); 
    ImGui::Spacing();
    CenteredText("免责声明", 2.1f);
    CenteredText("开发者也不会主动联系你\n", 1.2f);
    CenteredText("本项目也没有任何收费及内部\n", 1.2f);
    CenteredText("币子不会诋毁任何人及项目\n", 1.2f);
    CenteredText("网络上任何流言也要注意甄别\n", 1.2f);
    CenteredText("本项目仅供逆向学习研究\n", 1.2f);
    CenteredText("体验者的任何行为与作者无关", 1.2f);
    CenteredText("若有侵权违法行为,请联系我删除内容", 1.2f);
    CenteredText("请在24小时内删除本应用[下载代表同意本协议]", 1.2f);
    ImGui::EndChild();
}

    if (fabs(ImGui::GetWindowWidth() - currentWidth) > 5.0f) {
            ImGui::SetWindowSize(ImVec2(currentWidth, ImGui::GetWindowHeight()));
        }
        绘制.winWidth = ImGui::GetWindowWidth();
        绘制.winHeith = ImGui::GetWindowHeight();
        g_window = ImGui::GetCurrentWindow();
        ImGui::End();
    }
}
if (!悬浮窗) {
    悬浮球 = true;
    窗口状态 = true;
    IsBall = false;
    if (ImGui::FindWindowByName("币子")) {
        ImGui::SetWindowPos("悬浮图片", 绘制.Pos, ImGuiCond_Always);
    }
}
    drawEnd();
}

void 布局::开启悬浮窗(){
  timer WindowDrawing;
  WindowDrawing.SetFps(60);
  WindowDrawing.AotuFPS_init();
  WindowDrawing.setAffinity();
  while(true){
  初始化背景图();
    绘制悬浮窗();
    WindowDrawing.SetFps(绘制.按钮.当前帧率);
    WindowDrawing.AotuFPS();
    std::this_thread::sleep_for(1ms);
  }
}
