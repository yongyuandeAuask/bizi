/* 
 * by币子
 
 
 */

static TextureInfo 背景图纹理 = {0};
static bool 背景图已加载 = false;
static bool 背景图查找完成 = false; 
static bool 背景图初始化中 = false; 

#ifndef STBI_ONLY_JPEG
#define STBI_ONLY_JPEG
#endif

bool 加载本地图片(const char* 文件路径, TextureInfo* 纹理信息) {
    if (!纹理信息) {
        return false;
    }
    struct stat 文件状态;
    if (stat(文件路径, &文件状态) != 0) {
        return false;
    }
    const char* 扩展名 = strrchr(文件路径, '.');
    if (扩展名) {
        if (strcmp(扩展名, ".jpg") != 0 && strcmp(扩展名, ".jpeg") != 0 && 
            strcmp(扩展名, ".png") != 0) {
             return false;
        }
    }
    int 宽度, 高度, 通道数;
    unsigned char* 图片数据 = stbi_load(文件路径, &宽度, &高度, &通道数, STBI_rgb_alpha);
    if (!图片数据) {
    return false;
    }
    GLuint 纹理ID;
    glGenTextures(1, &纹理ID);
    glBindTexture(GL_TEXTURE_2D, 纹理ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 宽度, 高度, 0, GL_RGBA, GL_UNSIGNED_BYTE, 图片数据);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(图片数据);
    纹理信息->textureId = (ImTextureID)(intptr_t)纹理ID;
   return true;
}

void 初始化背景图() {
    if (背景图查找完成 || 背景图初始化中) return; 
    背景图初始化中 = true; 
     const char* 可能路径[] = {
     "/data/币子/背景.png",
     "/data/币子/背景.jpg",
     "/data/币子/背景.jpeg",
    };
    for (int i = 0; i < sizeof(可能路径) / sizeof(可能路径[0]); i++) {
        if (加载本地图片(可能路径[i], &背景图纹理)) {
            背景图已加载 = true;
              背景图查找完成 = true;
            背景图初始化中 = false;
            return;
        }
    }    
    背景图已加载 = false;
    背景图查找完成 = true; 
    背景图初始化中 = false;
}
void 绘制背景图(float 圆角大小 = 25.0f) {
    if (!背景图已加载) {
        return;
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 窗口位置 = ImGui::GetWindowPos();
    ImVec2 窗口尺寸 = ImGui::GetWindowSize();
    ImVec2 左上角 = 窗口位置;
    ImVec2 右下角 = ImVec2(窗口位置.x + 窗口尺寸.x, 窗口位置.y + 窗口尺寸.y);
    draw_list->AddImageRounded(背景图纹理.textureId, 左上角, 右下角, ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 圆角大小);
}
void 绘制背景图(ImVec2 center, float size, float 圆角大小 = 0.0f) {
    if (!背景图已加载) {
        return;
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 左上角 = {center.x - size / 2, center.y - size / 2};
    ImVec2 右下角 = {center.x + size / 2, center.y + size / 2};   
    draw_list->AddImageRounded(背景图纹理.textureId, 左上角, 右下角, ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 圆角大小);
}
void 绘制背景图(TextureInfo textureInfo, ImVec2 center, float size, float 圆角大小 = 0.0f) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 左上角 = {center.x - size / 2, center.y - size / 2};
    ImVec2 右下角 = {center.x + size / 2, center.y + size / 2};   
    draw_list->AddImageRounded(textureInfo.textureId, 左上角, 右下角, ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 圆角大小);
}
void 绘制背景图(TextureInfo textureInfo, ImVec2 左上角, ImVec2 右下角, float 圆角大小 = 0.0f) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();   
    draw_list->AddImageRounded(textureInfo.textureId, 左上角, 右下角, ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 圆角大小);
}



bool 背景图是否已加载() {
    if (!背景图查找完成) {
        初始化背景图();
    }
    return 背景图已加载;
}

TextureInfo 获取背景图纹理() {
    if (!背景图查找完成) {
        初始化背景图();
    }
    return 背景图纹理;
}

void 设置背景图纹理(TextureInfo 新纹理) {
    if (背景图已加载 && 背景图纹理.textureId) {
        GLuint 旧纹理ID = (GLuint)(intptr_t)背景图纹理.textureId;
        glDeleteTextures(1, &旧纹理ID);
    }
    背景图纹理 = 新纹理;
    背景图已加载 = true;
    背景图查找完成 = true;
}

void 重新加载背景图() {
    if (背景图已加载 && 背景图纹理.textureId) {
        GLuint 旧纹理ID = (GLuint)(intptr_t)背景图纹理.textureId;
        glDeleteTextures(1, &旧纹理ID);
        背景图纹理.textureId = 0;
    }
    背景图已加载 = false;
    背景图查找完成 = false;
    背景图初始化中 = false;
    初始化背景图(); 
}

void 清理背景图资源() {
    if (背景图已加载 && 背景图纹理.textureId) {
        GLuint 纹理ID = (GLuint)(intptr_t)背景图纹理.textureId;
        glDeleteTextures(1, &纹理ID);
        背景图纹理.textureId = 0;
        背景图已加载 = false;
        背景图查找完成 = false;
        背景图初始化中 = false;
        printf("背景图资源已清理\n");
    }
}

void 程序退出清理() {
    清理背景图资源();
    printf("程序资源清理完成\n");
}
