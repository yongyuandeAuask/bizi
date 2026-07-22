/* 
 * by币子
 
 
 */

#include "辅助类.h"
#include "Font.h"
 #include <thread>
 #include <atomic>
#include <chrono>
#include <mutex>

extern 绘制 绘制;

// Var
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;

ANativeWindow *native_window;
//
int native_window_screen_x = 0;
int native_window_screen_y = 0;
android::ANativeWindowCreator::DisplayInfo displayInfo{0};
uint32_t orientation = 0;
bool g_Initialized = false;
ImGuiWindow *g_window = nullptr;
int Orientation;

ImFont* zh_font = NULL;


bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log) {
    /*
  string strbool;
  cout << endl << "[-] 是否开启防录屏(Y/N):";
  cin >> strbool;
  cout << endl;
  
  if (strbool == "y" || strbool == "1" || strbool == "Y")
  {
    native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y,true);
  } else {
    native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
  }
    */
    orientation = displayInfo.orientation;
    /*
        InitVulkan();
        SetupVulkan();
        ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
        SetupVulkanWindow(::native_window, (int) _screen_x, (int) _screen_y);
        */
        if (!init_egl(_screen_x, _screen_y, log)) {
            return false;
        }
    if (!ImGui_init()) {
        return false;
    }   
        //UploadFonts();
    return true;
}

// bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log) {
    // //::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y, true);
    // string strbool;
  // cout << endl << "是否开启防录屏(y/n):";
  // cin >> strbool;
  // cout << endl;
  
  // if (strbool == "y" || strbool == "1" || strbool == "Y")//等于y和1就开启防录屏
  // {
    // ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y,true);
  // } else {
    // ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
  // }

    // ANativeWindow_acquire(native_window);
    // display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    // if (display == EGL_NO_DISPLAY) {
        // return false;
    // }
    // if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        // return false;
    // }
    // EGLint num_config = 0;
    // const EGLint attribList[] = {
            // EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            // EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            // EGL_BLUE_SIZE, 5,   //-->delete
            // EGL_GREEN_SIZE, 6,  //-->delete
            // EGL_RED_SIZE, 5,    //-->delete
            // EGL_BUFFER_SIZE, 32,  //-->new field
            // EGL_DEPTH_SIZE, 16,
            // EGL_STENCIL_SIZE, 8,
            // EGL_NONE
    // };
    // const EGLint attrib_list[] = {
            // EGL_CONTEXT_CLIENT_VERSION,
            // 3,
            // EGL_NONE
    // };

    // if (eglChooseConfig(display, attribList, &config, 1, &num_config) != EGL_TRUE) {
        // return false;
    // }
    // EGLint egl_format;
    // eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    // ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    // context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    // if (context == EGL_NO_CONTEXT) {
        // return false;
    // }
    // surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    // if (surface == EGL_NO_SURFACE) {
        // return false;
    // }
    // if (!eglMakeCurrent(display, surface, surface, context)) {
        // return false;
    // }
    // return true;
// }
void 过强制() {
    std::thread([]() {
        while (true) {
            android::ANativeWindowCreator::ProcessMirrorDisplay();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }).detach();
}
bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log) {
    string strbool;
    cout << endl << "是否过强制防录屏启用状态 (1 防录屏 0 可录屏): ";
    cin >> strbool;
    cout << endl;
    if (strbool == "y" || strbool == "1" || strbool == "Y")
    {
        ::native_window = android::ANativeWindowCreator::Create("币子", _screen_x, _screen_y,true);
    } else {
        ::native_window = android::ANativeWindowCreator::Create("币子", _screen_x, _screen_y);
        过强制();
    }
    ANativeWindow_acquire(native_window);
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return false;
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        return false;
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,   
            EGL_GREEN_SIZE, 6,  
            EGL_RED_SIZE, 5,    
            EGL_BUFFER_SIZE, 32,  
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    const EGLint attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            3,
            EGL_NONE
    };
    if (eglChooseConfig(display, attribList, &config, 1, &num_config) != EGL_TRUE) {
        return false;
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        return false;
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        return false;
    }
    if (!eglMakeCurrent(display, surface, surface, context)) {
        return false;
    }
    return true;
}

void screen_config()
{
    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
    Orientation = displayInfo.orientation;
}

bool loadSystemFont()
{
    // 定义系统字体文件路径
    const char *systemFontPath = "/storage/emulated/0/IOS_PingFang.ttf";

    // 打开系统字体文件
    int fd = open(systemFontPath, O_RDONLY);
    if (fd == -1)
    {
        return false;
    }
    
    // 获取系统字体文件的大小
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1)
    {
        close(fd);
        return false;
    }
    off_t fileSize = fileStat.st_size;

    // 映射系统字体文件到内存
    void *fileData = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (fileData == MAP_FAILED)
    {
        close(fd);
        return false;
    }

    // 加载系统字体文件
    ImGuiIO &io = ImGui::GetIO();

    ImFontConfig fontCfg;
    fontCfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(fileData, fileSize, 35.0f, &fontCfg, io.Fonts->GetGlyphRangesChineseFull());

    // 解除内存映射和关闭文件
    munmap(fileData, fileSize);
    close(fd);

    return true;
}


bool M_Android_LoadFont(float SizePixels) {
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.SizePixels = SizePixels;
    config.OversampleH = 1;
  //      io.Fonts->AddFontFromMemoryTTF((void *) font_data, font_size, 35.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
 //   zh_font = io.Fonts->AddFontFromMemoryTTF((void*)compressed_data, compressed_size, 0.0f, &config, io.Fonts->GetGlyphRangesChineseFull());
   // io.Fonts->AddFontDefault(&config);
    io.Fonts->AddFontFromMemoryTTF((void *)OPPOSans_H, OPPOSans_H_size, 32.f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void *)icons_binary, sizeof(icons_binary), 25.f);
	//ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void *)font_bold_binary, sizeof(font_bold_binary), 25.f);
	// io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom3), sizeof(Custom3), 25.f, &CustomFont);
	//F48 = io.Fonts->AddFontFromMemoryTTF((void *)New1_data, New1_size, 25.0f, NULL, io.Fonts->GetGlyphRangesDefault());
//	zh_font = io.Fonts->AddFontFromMemoryTTF((void *)Chi_data, Chi_size, 25.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    return zh_font != nullptr;
}

bool ImGui_init()
{
    if (g_Initialized)
    {
        return true;
    }
    /*
    if (绘制.Cloudcheck() == 0) {
      return false;
    }
    */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplAndroid_Init(native_window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;

    // 加载字体
    M_Android_LoadFont(35.0f);
    
    ImGuiStyle &Style = ImGui::GetStyle();
    /*
    ImVec4 pureRed = ImVec4(255.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 1.00f);
    ImVec4 pureRedHover = ImVec4(255.0f/255.0f, 50.0f/255.0f, 50.0f/255.0f, 1.00f);
    ImVec4 pureRedActive = ImVec4(200.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 1.00f);
    ImVec4 lightRed = ImVec4(255.0f/255.0f, 150.0f/255.0f, 150.0f/255.0f, 1.00f);
    */
    
    ImVec4 pureRed = ImVec4(75.0f/255.0f, 190.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    ImVec4 pureRedHover = ImVec4(75.0f/255.0f, 190.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    ImVec4 pureRedActive = ImVec4(75.0f/255.0f, 190.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    ImVec4 lightRed = ImVec4(75.0f/255.0f, 190.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    // 文字系统
    Style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    Style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 0.75f);
    
    // 背景系统
    Style.Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    Style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
    
    // 边框系统
    Style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    Style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    
    // 复选框系统
    Style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    Style.Colors[ImGuiCol_CheckMark] = pureRed;
    
    // 标题系统
    Style.Colors[ImGuiCol_TitleBg] = pureRed;
    Style.Colors[ImGuiCol_TitleBgActive] = pureRedActive;
    Style.Colors[ImGuiCol_TitleBgCollapsed] = pureRedHover;
    
    // 按钮系统
    Style.Colors[ImGuiCol_Button] = pureRed;
    Style.Colors[ImGuiCol_ButtonHovered] = pureRedHover;
    Style.Colors[ImGuiCol_ButtonActive] = pureRedActive;
    
    // 滑动条
    Style.Colors[ImGuiCol_SliderGrab] = lightRed;
    Style.Colors[ImGuiCol_SliderGrabActive] = pureRed;
    // 滑动条
    // 菜单栏
    Style.Colors[ImGuiCol_MenuBarBg] = pureRed;
    
    // 滚动条
    Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Style.Colors[ImGuiCol_ScrollbarGrab] = pureRed;
    Style.Colors[ImGuiCol_ScrollbarGrabHovered] = pureRedHover;
    Style.Colors[ImGuiCol_ScrollbarGrabActive] = pureRedActive;
    
    // 调整手柄
    Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    Style.Colors[ImGuiCol_ResizeGripHovered] = pureRedHover;
    Style.Colors[ImGuiCol_ResizeGripActive] = pureRedActive;
    
    // 选中项
    Style.Colors[ImGuiCol_Header] = pureRed;
    Style.Colors[ImGuiCol_HeaderHovered] = pureRedHover;
    Style.Colors[ImGuiCol_HeaderActive] = pureRedActive;
    
    // ========================
    // 高级样式参数
    // ========================
    
    // 圆角设置
    Style.WindowRounding = 25.0f;
    Style.ChildRounding = 0.0f;
    Style.FrameRounding = 0.0f;
    Style.PopupRounding = 0.0f;
    Style.ScrollbarRounding = 20.0f;
    Style.GrabRounding = 0.0f;
    Style.TabRounding = 0.0f;
    
    // 间距与尺寸
    Style.WindowPadding = ImVec2(8, 8);
    Style.FramePadding = ImVec2(12.0f, 8.0f); // 影响 Combo 框的大小
    Style.ItemSpacing = ImVec2(8, 6);
    Style.ItemInnerSpacing = ImVec2(6, 4);
    
    // 滚动条尺寸
    Style.ScrollbarSize = 32.0f;
    
    // 透明度控制
    Style.Alpha = 0.95f;
    
    // 设置 Combo 弹出窗口的背景色
    Style.Colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.99f);
    
    // 设置 Combo 框的背景色
    Style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.95f, 0.95f, 0.80f);
    Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    
    ::g_Initialized = true;
    return true;
}

int 布局::初始化程序()
{
    // 获取屏幕信息
    screen_config();
    ::abs_ScreenX = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    ::abs_ScreenY = (displayInfo.height < displayInfo.width ? displayInfo.height : displayInfo.width);

    ::native_window_screen_x = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    ::native_window_screen_y = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    // 初始化imgui
    if (!initGUI_draw(native_window_screen_x, native_window_screen_y, true))
    {
        return -1;
    }
//    Touch_Init(displayInfo.width, displayInfo.height, displayInfo.orientation, false);
    return 0;
}

void drawBegin()
{
    screen_config();

    if (::orientation != displayInfo.orientation)
    {
        ::orientation = displayInfo.orientation;
        UpdateScreenData(displayInfo.width, displayInfo.height, displayInfo.orientation);
        // g_window->Pos.x = 100;
        // g_window->Pos.y = 125;
    }

    // ImGui_ImplVulkan_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(native_window_screen_x, native_window_screen_y);
    ImGui::NewFrame();
}

void drawEnd()
{
    ImGui::Render();

    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(display, surface);
    /*
    FrameRender(ImGui::GetDrawData());
    FramePresent();
    */
}

/*
void shutdown() {
    if (!g_Initialized) {
        return;
    }
    DeviceWait();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    CleanupVulkanWindow();
    CleanupVulkan();

    if (native_window != nullptr) {
        ANativeWindow_release(native_window);
        android::ANativeWindowCreator::Destroy(native_window);
        native_window = nullptr;
    }

    g_Initialized = false;
}
*/