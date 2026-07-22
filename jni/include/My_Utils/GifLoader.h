/* 
 * by币子
 
 
 */

#ifndef GIF_IMAGE_LOADER_H
#define GIF_IMAGE_LOADER_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GLES3/gl3.h>  // 使用 OpenGL ES 3.0
#include <vector>
#include <chrono>
#include <iostream>
#include <fox_gif.h>  //c格式GIF数据头文件

struct ImageTexture {
    GLuint textureId;  // OpenGL 纹理 ID
    int width;         // 图像宽度
    int height;        // 图像高度
};

class GifImageLoader {
private:
    std::vector<ImageTexture> images;           // GIF 帧纹理列表
    std::vector<int> frameDelays;               // 每帧的延迟时间
    size_t currentFrame = 0;                    // 当前帧索引
    std::chrono::steady_clock::time_point lastFrameTime;  // 上一帧时间

    // 缓存 GIF 数据的哈希值，用于判断数据是否已加载
    size_t gifHash = 0;

    // 计算数据哈希值（简单的加权求和法）
    size_t CalculateHash(const unsigned char* data, size_t size) {
        size_t hash = 0;
        for (size_t i = 0; i < size; ++i) {
            hash = hash * 31 + data[i];
        }
        return hash;
    }

public:
    // 加载 GIF 数据并解析每一帧
    bool LoadGif(const unsigned char* data, size_t size) {
        // 如果数据未改变，则直接返回
        size_t newHash = CalculateHash(data, size);
        if (gifHash == newHash) return true;

        Cleanup();  // 清理已有数据
        gifHash = newHash;

        int width, height, frameCount, channels;
        int* delays = nullptr;  // 用于存储帧延迟
        unsigned char* imgData = stbi_load_gif_from_memory(
            data, size, &delays, &width, &height, &frameCount, &channels, 4);

        if (!imgData) {
            std::cerr << "Failed to load GIF data." << std::endl;
            return false;
        }

        // 每帧的像素数据大小
        size_t frameSize = width * height * 4;

        // 为每帧生成 OpenGL 纹理，并记录延迟
        for (int i = 0; i < frameCount; ++i) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imgData[i * frameSize]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            images.push_back({texture, width, height});
            frameDelays.push_back(delays ? delays[i] : 100);  // 如果未提供延迟，默认使用 100ms
        }

        stbi_image_free(imgData);

        lastFrameTime = std::chrono::steady_clock::now();  // 初始化时间点
        currentFrame = 0;                                  // 重置帧索引
        return true;
    }

    // 更新帧索引
    void UpdateFrame() {
        if (images.empty() || frameDelays.empty()) return;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime).count();

        // 使用当前帧的延迟时间判断是否切换帧
        if (elapsed > frameDelays[currentFrame]) {
            currentFrame = (currentFrame + 1) % images.size();
            lastFrameTime = now;
        }
    }

    // 渲染当前帧（带圆角）
    void RenderImage(ImVec2 size, float cornerRadius = 9999.0f) {
        if (images.empty()) return;

        // 获取当前帧的纹理 ID
        ImTextureID textureId = (ImTextureID)(uintptr_t)images[currentFrame].textureId;
        
        // 创建圆角矩形绘制列表
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        
        // 创建圆角矩形路径
        ImVec2 rectMin = pos;
        ImVec2 rectMax = ImVec2(pos.x + size.x, pos.y + size.y);
        
        // 使用 ImDrawList 绘制带圆角的图像
        drawList->AddImageRounded(textureId, rectMin, rectMax, 
                                 ImVec2(0, 0), ImVec2(1, 1), 
                                 IM_COL32_WHITE, cornerRadius);
        
        // 更新光标位置（占用相应空间）
        ImGui::Dummy(size);
    }

    // 清除所有纹理数据
    void Cleanup() {
        for (auto& texture : images) {
            glDeleteTextures(1, &texture.textureId);
        }
        images.clear();
        frameDelays.clear();
        gifHash = 0;  // 重置哈希值
    }

    // 析构函数，释放资源
    ~GifImageLoader() {
        Cleanup();
    }
};

// 单函数加载和渲染 GIF 数据（带圆角）
void LoadAndRenderGifImage(const unsigned char* data, size_t size, ImVec2 renderSize, float cornerRadius = 9999.0f) {
    static GifImageLoader gifLoader;  // 全局加载器实例

    // 如果 GIF 数据未加载，则加载它
    gifLoader.LoadGif(data, size);

    // 更新并渲染当前帧（带圆角）
    gifLoader.UpdateFrame();
    gifLoader.RenderImage(renderSize, cornerRadius);
}

#endif  // GIF_IMAGE_LOADER_H
