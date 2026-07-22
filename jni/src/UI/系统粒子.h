/* 
 * by币子
 
 
 */

enum ParticleStyle {
    PARTICLE_SNOW = 0,    
    PARTICLE_COLORFUL,    
    PARTICLE_LEAF         
};

static int currentParticleStyle = PARTICLE_SNOW;
static constexpr int 最大粒子数 = 200;
static struct 雪花粒子 {
    ImVec2 位置;
    ImVec2 速度;
    float 大小;
    ImColor 颜色;
    float 生命周期;
    
} 
粒子数组[最大粒子数];
static bool 粒子系统已初始化 = false;
static float 粒子生成计时器 = 0.0f;
static constexpr int 最大落叶粒子数 = 15; 

static struct 落叶粒子 {
    ImVec2 位置;
    ImVec2 速度;
    float 大小;           
    float 旋转角度;
    float 旋转速度;
    float 生命周期;
    int 纹理类型;         
    bool 已落地;
    float 摆动幅度;       
    float 摆动频率;
}

落叶数组[最大落叶粒子数];
static TextureInfo 落叶纹理[3] = {0}; 
static bool 落叶纹理已加载 = false;
static bool 落叶纹理查找完成 = false;

ImColor 随机彩色() {
    int 颜色类型 = rand() % 8;
    switch (颜色类型) {
        case 0: return ImColor(255, 50, 50, 200);   
        case 1: return ImColor(50, 255, 50, 200);   
        case 2: return ImColor(50, 50, 255, 200);   
        case 3: return ImColor(255, 255, 50, 200);  
        case 4: return ImColor(255, 50, 255, 200);  
        case 5: return ImColor(50, 255, 255, 200);  
        case 6: return ImColor(255, 150, 50, 200);  
        case 7: return ImColor(50, 150, 255, 200);  
        default: return ImColor(200, 50, 150, 200); 
    }
}

void 初始化落叶纹理系统() {
    if (落叶纹理查找完成) return;
    const char* 可能路径[] = {
        "/data/币子/飘落.png",
        "/data/币子/飘落.jpg",
        "/data/币子/飘落.jpeg",
    };
    int 加载成功数量 = 0;
    for (int i = 0; i < sizeof(可能路径) / sizeof(可能路径[0]) && 加载成功数量 < 3; i++) {
        TextureInfo tempTexture = {0};
        if (加载本地图片(可能路径[i], &tempTexture)) {
            落叶纹理[加载成功数量] = tempTexture;
            加载成功数量++;
            if (加载成功数量 == 1) {
                for (int j = 1; j < 3; j++) {
                    落叶纹理[j] = tempTexture;
                }
                加载成功数量 = 3;
                break;
            }
        }
    }
    if (加载成功数量 > 0) {
        落叶纹理已加载 = true;
    }
    落叶纹理查找完成 = true;
}
void 初始化粒子系统() {
    for (int i = 0; i < 最大粒子数; ++i) {
        粒子数组[i].位置 = ImVec2(0, 0);
        粒子数组[i].速度 = ImVec2(0, 0);
        粒子数组[i].大小 = 0;
        粒子数组[i].颜色 = 随机彩色();
        粒子数组[i].生命周期 = 0.0f;
    }
    粒子系统已初始化 = true;
}
void 初始化彩色粒子系统() {
    for (int i = 0; i < 最大粒子数; ++i) {
        粒子数组[i].位置 = ImVec2(rand() % (int)abs_ScreenX, rand() % (int)abs_ScreenY);
        粒子数组[i].速度 = ImVec2((rand() % 80 - 50) * 0.05f, (rand() % 30 + 10) * 0.3f);
        粒子数组[i].大小 = (rand() % 5 + 5) * 0.1f;
        粒子数组[i].颜色 = 随机彩色();
        粒子数组[i].生命周期 = (rand() % 100 + 100) * 0.01f;
    }
}
void 初始化落叶粒子系统() {
    if (!落叶纹理查找完成) {
        初始化落叶纹理系统();
    }
    if (!落叶纹理已加载) {
        return;
    }
    for (int i = 0; i < 最大落叶粒子数; ++i) {
        落叶数组[i].位置 = ImVec2(rand() % (int)abs_ScreenX,rand() % (int)(abs_ScreenY * 0.3f));
        落叶数组[i].速度 = ImVec2((rand() % 40 - 20) * 0.1f, (rand() % 30 + 15) * 0.1f);
        落叶数组[i].大小 = (rand() % 20 + 25) * 0.1f; 
        落叶数组[i].旋转角度 = (rand() % 360) * IM_PI / 180.0f;
        落叶数组[i].旋转速度 = (rand() % 15 - 7) * 0.01f; 
        落叶数组[i].生命周期 = (rand() % 300 + 200) * 0.01f; 
        落叶数组[i].纹理类型 = rand() % 3;
        落叶数组[i].已落地 = false;
        落叶数组[i].摆动幅度 = (rand() % 10 + 5) * 0.1f; 
        落叶数组[i].摆动频率 = (rand() % 5 + 3) * 0.1f;  
    }
}
void 切换粒子样式(int newStyle) {
    currentParticleStyle = newStyle;
    if (粒子系统已初始化) {
        switch (currentParticleStyle) {
            case PARTICLE_SNOW:
                初始化粒子系统();
                break;
            case PARTICLE_COLORFUL:
                初始化彩色粒子系统();
                break;
            case PARTICLE_LEAF:
                初始化落叶粒子系统();
                break;
        }
    }
}
void 更新落叶粒子系统(ImVec2 窗口位置, ImVec2 窗口尺寸) {
    if (!落叶纹理已加载) {
        return;
    }
    float 底部边界 = 窗口位置.y + 窗口尺寸.y;
    float 当前时间 = ImGui::GetTime();
    static float 落叶生成计时器 = 0.0f;
    落叶生成计时器 += ImGui::GetIO().DeltaTime;
    if (落叶生成计时器 >= 0.1f) { 
        落叶生成计时器 = 0.0f;
        for (int i = 0; i < 最大落叶粒子数; ++i) {
            if (落叶数组[i].生命周期 <= 0.0f) {
                落叶数组[i].位置 = ImVec2(
                    窗口位置.x + (rand() % (int)窗口尺寸.x),
                    窗口位置.y - 50 
                );
                落叶数组[i].速度 = ImVec2(
                    (rand() % 40 - 20) * 0.1f,
                    (rand() % 30 + 15) * 0.1f
                );
                落叶数组[i].大小 = (rand() % 20 + 25) * 0.1f; 
                落叶数组[i].旋转角度 = 0;
                落叶数组[i].旋转速度 = (rand() % 15 - 7) * 0.01f;
                落叶数组[i].生命周期 = (rand() % 300 + 200) * 0.01f; 
                落叶数组[i].纹理类型 = rand() % 3;
                落叶数组[i].已落地 = false;
                落叶数组[i].摆动幅度 = (rand() % 10 + 5) * 0.1f;
                落叶数组[i].摆动频率 = (rand() % 5 + 3) * 0.1f;
                break;
            }
        }
    }
    for (int i = 0; i < 最大落叶粒子数; ++i) {
        if (落叶数组[i].生命周期 > 0.0f) {
            if (!落叶数组[i].已落地) {
                float 摆动偏移 = sinf(当前时间 * 落叶数组[i].摆动频率 + i) * 落叶数组[i].摆动幅度;
                落叶数组[i].位置.x += (落叶数组[i].速度.x + 摆动偏移) * ImGui::GetIO().DeltaTime * 60.0f;
                落叶数组[i].位置.y += 落叶数组[i].速度.y * ImGui::GetIO().DeltaTime * 60.0f;
                落叶数组[i].旋转角度 += 落叶数组[i].旋转速度;
                if (落叶数组[i].位置.y >= 底部边界) {
                    落叶数组[i].已落地 = true;
                    落叶数组[i].生命周期 = 0.5f; 
                }
                落叶数组[i].生命周期 -= ImGui::GetIO().DeltaTime * 0.3f; 
            } else {
                落叶数组[i].生命周期 -= ImGui::GetIO().DeltaTime * 1.0f;
            }
            if (落叶数组[i].生命周期 <= 0.0f) {
                落叶数组[i].生命周期 = 0.0f;
            }
        }
    }
}
void 绘制落叶粒子系统() {
    if (!落叶纹理已加载) {
        return;
    }
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    for (int i = 0; i < 最大落叶粒子数; ++i) {
        if (落叶数组[i].生命周期 > 0.0f && 落叶纹理[落叶数组[i].纹理类型].textureId) {
            float 透明度 = 落叶数组[i].生命周期;
            if (透明度 > 1.0f) 透明度 = 1.0f;
            float 纹理大小 = 落叶数组[i].大小 * 25.0f; 
            float 余弦 = cosf(落叶数组[i].旋转角度);
            float 正弦 = sinf(落叶数组[i].旋转角度);
            ImVec2 顶点[4];
            float 半尺寸 = 纹理大小 / 2.0f;
            顶点[0] = ImVec2(-半尺寸, -半尺寸);
            顶点[1] = ImVec2(半尺寸, -半尺寸);
            顶点[2] = ImVec2(半尺寸, 半尺寸);
            顶点[3] = ImVec2(-半尺寸, 半尺寸);
            for (int j = 0; j < 4; j++) {
                float x = 顶点[j].x * 余弦 - 顶点[j].y * 正弦;
                float y = 顶点[j].x * 正弦 + 顶点[j].y * 余弦;
                顶点[j] = ImVec2(落叶数组[i].位置.x + x, 落叶数组[i].位置.y + y);
            }
            int 最终透明度 = (int)(透明度 * 255);
            最终透明度 = ImClamp(最终透明度, 0, 255);
            drawList->AddImageQuad(落叶纹理[落叶数组[i].纹理类型].textureId,顶点[0], 顶点[1], 顶点[2], 顶点[3],ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1),IM_COL32(255, 255, 255, 最终透明度));
        }
    }
}
void 更新粒子系统(ImVec2 窗口位置, ImVec2 窗口尺寸, bool 窗口打开状态) {
    if (!粒子系统已初始化) {
        初始化粒子系统();
    }
    if (!窗口打开状态) {
        return;
    }
    float 当前时间 = ImGui::GetTime();
    粒子生成计时器 += ImGui::GetIO().DeltaTime;
    switch (currentParticleStyle) {
        case PARTICLE_SNOW:
            if (粒子生成计时器 >= 0.01f) {
                粒子生成计时器 = 0.0f;
                for (int i = 0; i < 最大粒子数; ++i) {
                    if (粒子数组[i].生命周期 <= 0.0f) {
                        粒子数组[i].位置 = ImVec2(
                            窗口位置.x + (rand() % (int)窗口尺寸.x),
                            窗口位置.y - 10
                        );
                        粒子数组[i].速度 = ImVec2((rand() % 100 - 50) * 0.1f * 0.7f,(rand() % 50 + 50) * 0.5f * 0.7f);
                        粒子数组[i].大小 = (rand() % 10 + 5) * 0.5f;
                        粒子数组[i].颜色 = 随机彩色();
                        粒子数组[i].生命周期 = (rand() % 100 + 50) * 0.01f;
                        break;
                    }
                }
            }
            break;
        case PARTICLE_COLORFUL:
            if (粒子生成计时器 >= 0.02f) {
                粒子生成计时器 = 0.0f;
                for (int i = 0; i < 最大粒子数; ++i) {
                    if (粒子数组[i].生命周期 <= 0.0f) {
                        粒子数组[i].位置 = ImVec2(窗口位置.x + (rand() % (int)窗口尺寸.x),窗口位置.y + (rand() % (int)窗口尺寸.y));
                        粒子数组[i].速度 = ImVec2((rand() % 90 - 80) * 0.1f,(rand() % 90 - 50) * 0.1f);
                        粒子数组[i].大小 = (rand() % 10 + 8) * 0.4f;
                        粒子数组[i].颜色 = 随机彩色();
                        粒子数组[i].生命周期 = (rand() % 150 + 100) * 0.01f;
                        break;
                    }
                }
            }
            break;
        case PARTICLE_LEAF:
            更新落叶粒子系统(窗口位置, 窗口尺寸);
            break;
    }
    if (currentParticleStyle != PARTICLE_LEAF) {
        for (int i = 0; i < 最大粒子数; ++i) {
            if (粒子数组[i].生命周期 > 0.0f) {
                float speedMultiplier = (currentParticleStyle == PARTICLE_COLORFUL) ? 0.6f : 0.7f;
                粒子数组[i].位置.x += 粒子数组[i].速度.x * ImGui::GetIO().DeltaTime * 60.0f * speedMultiplier;
                粒子数组[i].位置.y += 粒子数组[i].速度.y * ImGui::GetIO().DeltaTime * 60.0f * speedMultiplier;
                粒子数组[i].生命周期 -= ImGui::GetIO().DeltaTime;
                if (粒子数组[i].位置.y > 窗口位置.y + 窗口尺寸.y || 
                    粒子数组[i].位置.x < 窗口位置.x || 
                    粒子数组[i].位置.x > 窗口位置.x + 窗口尺寸.x ||
                    粒子数组[i].生命周期 <= 0.0f) {
                    粒子数组[i].生命周期 = 0.0f;
                }
            }
        }
    }
}
void 绘制粒子系统(bool 窗口打开状态) {    
    if (!窗口打开状态) {
        return;
    }
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    switch (currentParticleStyle) {
        case PARTICLE_SNOW:
            for (int i = 0; i < 最大粒子数; ++i) {
                if (粒子数组[i].生命周期 > 0.0f) {
                    float 透明度 = 粒子数组[i].生命周期;
                    if (透明度 > 1.0f) 透明度 = 1.0f;
                    ImColor 最终颜色 = 粒子数组[i].颜色;
                    最终颜色.Value.w = 透明度 * 0.8f;
                    drawList->AddCircleFilled(粒子数组[i].位置, 粒子数组[i].大小, 最终颜色);
                }
            }
            break;
        case PARTICLE_COLORFUL:
            for (int i = 0; i < 最大粒子数; ++i) {
                if (粒子数组[i].生命周期 > 0.0f) {
                    float 透明度 = 粒子数组[i].生命周期;
                    if (透明度 > 1.0f) 透明度 = 1.0f;
                    ImColor 最终颜色 = 粒子数组[i].颜色;
                    最终颜色.Value.w = 透明度 * 0.9f;
                    drawList->AddCircleFilled(粒子数组[i].位置, 粒子数组[i].大小, 最终颜色);
                }
            }
            break;
        case PARTICLE_LEAF:
            绘制落叶粒子系统();
            break;
    }
}
void 清理落叶纹理资源() {
    for (int i = 0; i < 3; i++) {
        if (落叶纹理[i].textureId) {
            GLuint 纹理ID = (GLuint)(intptr_t)落叶纹理[i].textureId;
            glDeleteTextures(1, &纹理ID);
            落叶纹理[i].textureId = 0;
        }
    }
    落叶纹理已加载 = false;
    落叶纹理查找完成 = false;
    printf("落叶纹理资源已清理\n");
}
