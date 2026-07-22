/* 
 * 由币子开源
 * 注入时间: 2026-01-09 16:45:26
 * 联系方式: 3398723610
 */

#include "imgui.h"
#include "imgui_internal.h" 
float tap页[150];
float speed = 1.5f;
void Lerp(float& a, const float& b, float t) {
    a += (b - a) * (t * speed);
}
template<typename T>
void Lerp(T& current, const T& target, float factor) {
    current = current + (target - current) * factor;
}

template<typename T>
void Lerp(T& current, const T& from, const T& to, float t) {
    current = from + (to - from) * t;
}


struct wc {
int Tab;
float beg, end;
float 动画, 动画目前, 动画目标, 动画增加速度, 动画减少速度, 动画最小值, 动画展开百分比;
bool 动画开关, 显示内容;
ImColor color;
};

map<string, wc> wh;
extern map<string, ImColor> Styles;
struct TabAnimationState {
    float animationPosition;
    float targetPosition;
    int lastSelectedIndex;
    float buttonWidth;
    float targetButtonWidth;
    bool isAnimating;
    std::vector<float> buttonPositions;
};

void VerticalTabBar(const vector<string>& names, float& selectedIndex, float buttonWidth = 180, float buttonHeight = 85, float highlightRounding = 20.0f, const char* uniqueId = "default") {
    ImGuiStyle& style = ImGui::GetStyle();
    const float extraSpacing = 0.0f;
    float totalHeight = names.size() * buttonHeight + (names.size() - 1) * (style.ItemSpacing.y + extraSpacing);
    ImGui::BeginChild("VerticalTabs", ImVec2(buttonWidth, 730), false, ImGuiWindowFlags_NoBackground);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f,0.9f,0.9f,0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f,0.8f,0.8f,0.2f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    static std::map<std::string, TabAnimationState> animationStates;
    if (animationStates.find(uniqueId) == animationStates.end()) {
        TabAnimationState newState;
        newState.animationPosition = 0;
        newState.targetPosition = 0;
        newState.lastSelectedIndex = -1;
        newState.buttonWidth = buttonWidth;
        newState.targetButtonWidth = buttonWidth;
        newState.isAnimating = false;
        animationStates[uniqueId] = newState;
    }
    TabAnimationState& state = animationStates[uniqueId];
    if (fabs(state.buttonWidth - state.targetButtonWidth) > 0.1f) {
        Lerp(state.buttonWidth, state.targetButtonWidth, 0.15f);
        state.isAnimating = true;
    } else {
        state.buttonWidth = state.targetButtonWidth;
        state.isAnimating = false;
    }
    if (fabs(state.targetButtonWidth - buttonWidth) > 0.1f) {
        state.targetButtonWidth = buttonWidth;
    }
    float actualButtonWidth = state.buttonWidth;
    vector<float> buttonPositions(names.size());
    if (state.lastSelectedIndex == -1) {
        state.animationPosition = 0;
        state.targetPosition = 0;
    }
    Lerp(state.animationPosition, state.targetPosition, 0.08f);
    float dynamicSliderWidth = actualButtonWidth;
    if (actualButtonWidth < 150.0f) {
        dynamicSliderWidth = actualButtonWidth * 0.9f; 
    } else if (actualButtonWidth > 200.0f) {
        dynamicSliderWidth = actualButtonWidth * 0.95f; 
    }
    float sliderOffsetX = (actualButtonWidth - dynamicSliderWidth) * 0.5f;
    ImVec2 rectMin = ImGui::GetWindowPos();
    rectMin.x += sliderOffsetX; 
    rectMin.y += state.animationPosition;
    ImVec2 rectMax = ImVec2(rectMin.x + dynamicSliderWidth, rectMin.y + buttonHeight);
    // ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, ImColor(255, 0, 0, 200), highlightRounding);
    ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, ImColor(75, 190, 255, 255), highlightRounding);
    for (int i = 0; i < names.size(); i++) {
        buttonPositions[i] = ImGui::GetCursorPosY();
        ImVec2 buttonSize(actualButtonWidth, buttonHeight);
        if (ImGui::InvisibleButton(names[i].c_str(), buttonSize)) {
            if ((int)selectedIndex != i) {
                state.targetPosition = buttonPositions[i];
                state.lastSelectedIndex = (int)selectedIndex;
                selectedIndex = (float)i;
            }
        }
        ImVec2 buttonMin = ImGui::GetItemRectMin();
        ImVec2 buttonMax = ImGui::GetItemRectMax();
        ImGui::SetWindowFontScale(1.02f);
        const float textPadding = 8.0f;
        ImU32 textColor;
        if (i == selectedIndex) {
            textColor = IM_COL32(0, 0, 0, 255); 
         }
        ImVec2 textSize = ImGui::CalcTextSize(names[i].c_str());
        ImVec2 textPos(buttonMin.x + (actualButtonWidth - textSize.x) * 0.5f,buttonMin.y + (buttonHeight - textSize.y) * 0.5f);
        ImGui::GetWindowDrawList()->AddText(textPos, textColor, names[i].c_str());
        if (i < names.size() - 1) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + style.ItemSpacing.y);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + extraSpacing);
        }
    }
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    ImGui::EndChild();
}

bool M_CustomSliderFloatvv(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f", int slider_type = 0) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    float total_height = 75.0f;
    float total_width = -1; 
    float horizontal_margin = 20.0f; 
    float vertical_margin = 15.0f;   
    float slider_height = 12.0f;
    float grab_radius = 15.0f;
    float available_width = ImGui::GetContentRegionAvail().x;
    float slider_width = available_width - horizontal_margin * 2;
    ImRect total_bb(pos, ImVec2(pos.x + available_width, pos.y + total_height));
    float slider_vertical_offset = 6.5f; 
    ImVec2 slider_pos = ImVec2(
        pos.x + horizontal_margin,
        pos.y + total_height - vertical_margin - slider_height + slider_vertical_offset
    );
    ImRect slider_bb(slider_pos, ImVec2(slider_pos.x + slider_width, slider_pos.y + slider_height));
    ImRect touch_bb = slider_bb;
    touch_bb.Expand(ImVec2(0, 10.0f));
    ImGui::ItemSize(total_bb, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &touch_bb))
        return false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(touch_bb, id, &hovered, &held);
    if (held) {
        float normalized_pos = ImClamp((g.IO.MousePos.x - slider_bb.Min.x) / slider_bb.GetWidth(), 0.0f, 1.0f);
        *v = ImLerp(v_min, v_max, normalized_pos);
    }
    float normalized_value = ImClamp((*v - v_min) / (v_max - v_min), 0.0f, 1.0f);
    float grab_pos = ImLerp(slider_bb.Min.x + grab_radius, slider_bb.Max.x - grab_radius, normalized_value);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImU32 background_color = IM_COL32(245, 245, 245, 200);
    float corner_radius = 12.0f;
    draw_list->AddRectFilled(total_bb.Min, total_bb.Max, background_color, corner_radius);
    ImU32 track_bg_color = IM_COL32(180, 180, 180, 255);
    ImU32 track_fill_color = IM_COL32(75, 190, 255, 200); // 修改后的滑块填充色
ImU32 grab_border_color = IM_COL32(75, 190, 255, 200); // 修改后的抓取点边框色
    ImU32 grab_color;
    if (held) {
        grab_color = IM_COL32(255, 0, 0, 200); 
    } else {
        grab_color = hovered ? IM_COL32(220, 220, 220, 255) : IM_COL32(240, 240, 240, 255);
    }
    
    if (held) {
        grab_color = IM_COL32(255, 0, 0, 200); 
    } else {
        grab_color = hovered ? IM_COL32(220, 220, 220, 255) : IM_COL32(240, 240, 240, 255);
    }
    float track_rounding = slider_height * 0.5f;
    draw_list->AddRectFilled(slider_bb.Min, slider_bb.Max, track_bg_color, track_rounding);
    if (normalized_value > 0.0f) {
        ImRect filled_bb(slider_bb.Min, ImVec2(grab_pos, slider_bb.Max.y));
        draw_list->AddRectFilled(filled_bb.Min, filled_bb.Max, track_fill_color, track_rounding);
    }
    ImVec2 grab_center = ImVec2(grab_pos, (slider_bb.Min.y + slider_bb.Max.y) * 0.5f);
    draw_list->AddCircleFilled(grab_center, grab_radius, grab_border_color);
    float inner_radius = grab_radius * 0.6f;
    draw_list->AddCircleFilled(grab_center, inner_radius, IM_COL32(255, 255, 255, 255));
    if (label_size.x > 0.0f) {
        ImVec2 label_pos = ImVec2(pos.x + horizontal_margin,pos.y + 5.0f  );
        ImU32 text_color = IM_COL32(0, 0, 0, 255);
        draw_list->AddText(label_pos, text_color, label);
    }
    // char value_buf[64];
    // ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *v);
    // ImVec2 value_size = ImGui::CalcTextSize(value_buf);
    // ImVec2 value_pos = ImVec2(slider_bb.Max.x - value_size.x,pos.y + 5.0f);
    // ImU32 value_color = IM_COL32(0, 0, 0, 255);
    // draw_list->AddText(value_pos, value_color, value_buf);
    char value_buf[64];
    ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *v);
    ImVec2 value_size = ImGui::CalcTextSize(value_buf);
    
    // 添加圆角矩形背景
    float bg_padding_x = 10.0f; // 水平内边距
    float bg_padding_y = 4.0f;  // 垂直内边距
    float bg_rounding = 50.0f;  // 圆角半径
    ImVec2 bg_min = ImVec2(slider_bb.Max.x - value_size.x - bg_padding_x, pos.y + 5.0f - bg_padding_y);
    ImVec2 bg_max = ImVec2(slider_bb.Max.x + bg_padding_x, pos.y + 5.0f + value_size.y + bg_padding_y);
    // ImU32 bg_color = IM_COL32(0, 0, 0, 128); // 淡透明黑色，RGBA(0,0,0,128)
    ImU32 bg_color = IM_COL32(0, 0, 0, 8); // 淡透明黑色，RGBA(0,0,0,128)
    draw_list->AddRectFilled(bg_min, bg_max, bg_color, bg_rounding);
    
    // 调整文本位置至背景中心
    ImVec2 value_pos = ImVec2(
        bg_min.x + (bg_max.x - bg_min.x - value_size.x) * 0.5f,
        bg_min.y + (bg_max.y - bg_min.y - value_size.y) * 0.5f
    );
    ImU32 value_color = IM_COL32(255, 255, 255, 255); // 将文本颜色改为白色以提高在黑色背景上的可读性
    draw_list->AddText(value_pos, value_color, value_buf);
    return pressed;
}
string getBeijingTimeString() {
std::time_t now = std::time(nullptr);
std::tm* now_tm = std::gmtime(&now); 
now_tm->tm_hour += 8;
now_tm->tm_isdst = 0; 
if (now_tm->tm_hour >= 24) {
now_tm->tm_hour -= 24;
now_tm->tm_mday += 1;
}
char buffer[20];
std::strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", now_tm); 
return std::string(buffer);
}
bool M_CircularButton(const char* label, const ImVec2& size_arg = ImVec2(0 , 0), ImGuiButtonFlags flags = 0) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    float window_available_width = ImGui::GetContentRegionAvail().x;
    float horizontal_margin = 20.0f;
    float total_width = window_available_width - 2 * horizontal_margin;
    total_width = ImMax(total_width, 300.0f);
    ImVec2 size = ImGui::CalcItemSize(size_arg, total_width, 61.0f);
    float total_height = size.y;
    float vertical_padding = 15.0f;
    ImRect total_bb(pos, ImVec2(pos.x + total_width, pos.y + total_height));
    ImGui::ItemSize(total_bb, vertical_padding);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float shadow_offset_x = 2.0f;
    float shadow_offset_y = 2.0f;
    float shadow_spread = 1.0f;
    float corner_radius = 12.0f;
    ImVec2 shadow_min = ImVec2(total_bb.Min.x + shadow_offset_x, total_bb.Min.y + shadow_offset_y);
    ImVec2 shadow_max = ImVec2(total_bb.Max.x + shadow_offset_x, total_bb.Max.y + shadow_offset_y);
    shadow_min.x -= shadow_spread;
    shadow_min.y -= shadow_spread;
    shadow_max.x += shadow_spread;
    shadow_max.y += shadow_spread;
    draw_list->AddRectFilled(shadow_min, shadow_max, IM_COL32(0, 0, 0, 20), corner_radius);
    ImVec2 shadow2_min = ImVec2(shadow_min.x + 0.5f, shadow_min.y + 0.5f);
    ImVec2 shadow2_max = ImVec2(shadow_max.x + 0.5f, shadow_max.y + 0.5f);
    draw_list->AddRectFilled(shadow2_min, shadow2_max, IM_COL32(0, 0, 0, 15), corner_radius);
    ImVec2 shadow3_min = ImVec2(shadow2_min.x + 0.5f, shadow2_min.y + 0.5f);
    ImVec2 shadow3_max = ImVec2(shadow_max.x + 0.5f, shadow_max.y + 0.5f);
    draw_list->AddRectFilled(shadow3_min, shadow3_max, IM_COL32(0, 0, 0, 8), corner_radius);
    ImU32 background_color = IM_COL32(255, 255, 255, 255);
    draw_list->AddRectFilled(total_bb.Min, total_bb.Max, background_color, corner_radius);
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 label_pos = ImVec2(
        pos.x + horizontal_margin + (total_width - 2 * horizontal_margin - label_size.x) * 0.5f,
        pos.y + (total_height - label_size.y) * 0.5f
    );
    ImU32 text_color = pressed ? IM_COL32(255, 255, 255, 255) : 
                     hovered ? IM_COL32(0, 120, 215, 255) : 
                     IM_COL32(0, 0, 0, 255);
    if (pressed) {
        ImU32 pressed_color = IM_COL32(0, 120, 215, 255);
        draw_list->AddRectFilled(total_bb.Min, total_bb.Max, pressed_color, corner_radius);
    } else if (hovered) {
        draw_list->AddRect(total_bb.Min, total_bb.Max, IM_COL32(0, 120, 215, 100), corner_radius, 0, 1.5f);
    }
    draw_list->AddText(label_pos, text_color, label);
    if (hovered && !held) {
        draw_list->AddRect(total_bb.Min, total_bb.Max, IM_COL32(240, 240, 240, 255), corner_radius, 0, 1.5f);
    }
    return pressed;
}
void fanhua(const vector<string>& names, float& 参数, float 居中 = 0, float 按钮x = 130, float 按钮y = 75, float 水平间距 = 10) {
    ImVec2 按钮 = ImVec2(按钮x, 按钮y);
    ImVec2 差距 = ImVec2(水平间距, 0);
    float 总宽度 = 按钮.x * names.size() + 水平间距 * (names.size() - 1);
    ImVec2 窗口(总宽度, 按钮.y);
    wc& name = wh[names[0].c_str()];
    if (name.动画开关 == false) {
        name.动画开关 = true;
        name.动画目前 = 0;
        name.动画目标 = 0;
        name.beg = 0;
        name.end = 0;
        name.Tab = 0;
        参数 = 0;
    }
    if (居中) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x - 总宽度)/2 + 110.0f, ImGui::GetWindowPos().y + ImGui::GetCursorPos().y)); 
    } else {
        ImVec2 currentPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(currentPos.x + 110.0f, currentPos.y));
    }
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.85f, 0.85f, 0.85f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.92f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.85f, 0.87f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 9999.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::BeginChild(names[0].c_str(), 窗口, true);
    ImVec2 childMin = ImGui::GetWindowPos();
    ImVec2 childMax = ImVec2(childMin.x + 总宽度, childMin.y + 窗口.y);
    ImGui::GetWindowDrawList()->AddRectFilled(childMin, childMax, IM_COL32(75, 190, 255, 255), 9999.0f);
    ImGui::GetWindowDrawList()->AddRect(childMin, childMax, IM_COL32(75, 190, 255, 255), 999.0f, 0, 1.0f);
    vector<ImVec2> buttonMins(names.size());
    vector<ImVec2> buttonMaxs(names.size());
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
    for (int i = 0; i < names.size(); i++) {
        if (i > 0) {
            ImGui::SameLine(0, 水平间距);
        }
        if (ImGui::Button(names[i].c_str(), ImVec2(按钮.x, 按钮.y))) {
            if (name.Tab != i) {
                name.Tab = i;
                参数 = i;
                name.end = i * (按钮.x + 水平间距);
            }
        }
        buttonMins[i] = ImGui::GetItemRectMin();
        buttonMaxs[i] = ImGui::GetItemRectMax();
    }
    ImGui::PopStyleColor();
    float 动画速度 = 0.15f;
    name.beg = ImLerp(name.beg, name.end, 动画速度);
    if (fabs(name.beg - name.end) < 0.5f) {
        name.beg = name.end;
    }
    if (name.Tab >= 0 && name.Tab < names.size()) {
        float indicatorX = childMin.x + name.beg;
        float indicatorY = childMin.y;
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(indicatorX, indicatorY),ImVec2(indicatorX + 按钮.x, indicatorY + 按钮.y),ImColor(255, 0, 0, 255),9999.0f);
    }
    for (int i = 0; i < names.size(); i++) {
        string bynh = names[i];
        ImVec2 min = buttonMins[i];
        ImVec2 max = buttonMaxs[i];
        float px = min.x + (max.x - min.x) / 2;
        float py = min.y + (max.y - min.y) / 2;
        auto textSize_bynh = ImGui::GetFont()->CalcTextSizeA(32, FLT_MAX, -1, bynh.c_str(), NULL, NULL);
        ImVec2 textPos = ImVec2(px - (textSize_bynh.x / 2), py - (textSize_bynh.y / 2));
        ImU32 textColor;
        if (name.Tab == i) {
            textColor = IM_COL32(255, 255, 255, 255); 
        } else {
            textColor = IM_COL32(128, 128, 128, 255); 
        }
        ImGui::GetWindowDrawList()->AddText(NULL, 32, textPos, textColor, bynh.c_str());
    }
    ImGui::EndChild();
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(4);
}

bool M_CircularCheckbox(const char* label, bool* v) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    float total_width = 315.0f;  
    float total_height = 84.0f;  
    float switch_width = 85.0f;      
    float switch_height = 42.0f;     
    float switch_knob_radius = 15.0f; 
    float horizontal_margin = 20.0f;
    float vertical_padding = 15.0f;
    float spacing = 15.0f;
    ImRect total_bb(pos, ImVec2(pos.x + total_width, pos.y + total_height));
    ImGui::ItemSize(total_bb, vertical_padding);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImU32 background_color = IM_COL32(245, 245, 245, 200);
    float corner_radius = 12.0f;
    draw_list->AddRectFilled(total_bb.Min, total_bb.Max, background_color, corner_radius);
    ImU32 border_color = IM_COL32(200, 200, 200, 255);
    draw_list->AddRect(total_bb.Min, total_bb.Max, border_color, corner_radius, 0, 1.5f);
    if (hovered) {
        draw_list->AddRect(total_bb.Min, total_bb.Max, IM_COL32(150, 150, 150, 255), corner_radius, 0, 1.5f);
    }
    ImVec2 switch_pos = ImVec2(pos.x + total_width - horizontal_margin - switch_width,pos.y + (total_height - switch_height) * 0.5f);
    ImRect switch_bb(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height));
    ImVec2 label_pos = ImVec2(pos.x + horizontal_margin,pos.y + (total_height - label_size.y) * 0.5f);
    ImU32 track_color, knob_color;
    if (*v) {
        track_color = IM_COL32(135, 190, 235, 255); // 修改后的激活轨迹颜色
        knob_color = IM_COL32(75, 190, 255, 255);  // 修改后的激活旋钮颜色
    } else {
        track_color = IM_COL32(180, 180, 180, 255);
        knob_color = IM_COL32(220, 220, 220, 255);
    }
    float track_rounding = switch_height * 0.5f;
    draw_list->AddRectFilled(switch_bb.Min, switch_bb.Max, track_color, track_rounding);
    float knob_x;
    if (*v) {
        knob_x = switch_bb.Max.x - switch_knob_radius - 6.0f; 
    } else {
        knob_x = switch_bb.Min.x + switch_knob_radius + 6.0f; 
    }
    ImVec2 knob_center = ImVec2(knob_x, (switch_bb.Min.y + switch_bb.Max.y) * 0.5f);
    draw_list->AddCircleFilled(knob_center, switch_knob_radius, knob_color);
    ImU32 text_color = IM_COL32(0, 0, 0, 255);
    draw_list->AddText(label_pos, text_color, label);
    return pressed;
}
bool M_CircularCheckbox3(const char* label, bool* v) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    float total_width = 210.0f;  
    float total_height = 84.0f;  
    float switch_width = 85.0f;      
    float switch_height = 42.0f;     
    float switch_knob_radius = 15.0f; 
    float horizontal_margin = 20.0f;
    float vertical_padding = 15.0f;
    float spacing = 15.0f;
    ImRect total_bb(pos, ImVec2(pos.x + total_width, pos.y + total_height));
    ImGui::ItemSize(total_bb, vertical_padding);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImU32 background_color = IM_COL32(245, 245, 245, 200);
    float corner_radius = 12.0f;
    draw_list->AddRectFilled(total_bb.Min, total_bb.Max, background_color, corner_radius);
    ImU32 border_color = IM_COL32(200, 200, 200, 255);
    draw_list->AddRect(total_bb.Min, total_bb.Max, border_color, corner_radius, 0, 1.5f);
    if (hovered) {
        draw_list->AddRect(total_bb.Min, total_bb.Max, IM_COL32(150, 150, 150, 255), corner_radius, 0, 1.5f);
    }
    ImVec2 switch_pos = ImVec2(pos.x + total_width - horizontal_margin - switch_width,pos.y + (total_height - switch_height) * 0.5f);
    ImRect switch_bb(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height));
    ImVec2 label_pos = ImVec2(pos.x + horizontal_margin,pos.y + (total_height - label_size.y) * 0.5f);
    ImU32 track_color, knob_color;
    if (*v) {
        track_color = IM_COL32(135, 190, 235, 255); // 修改后的激活轨迹颜色
        knob_color = IM_COL32(75, 190, 255, 255);  // 修改后的激活旋钮颜色
    } else {
        track_color = IM_COL32(180, 180, 180, 255);
        knob_color = IM_COL32(220, 220, 220, 255);
    }
    float track_rounding = switch_height * 0.5f;
    draw_list->AddRectFilled(switch_bb.Min, switch_bb.Max, track_color, track_rounding);
    float knob_x;
    if (*v) {
        knob_x = switch_bb.Max.x - switch_knob_radius - 6.0f; 
    } else {
        knob_x = switch_bb.Min.x + switch_knob_radius + 6.0f; 
    }
    ImVec2 knob_center = ImVec2(knob_x, (switch_bb.Min.y + switch_bb.Max.y) * 0.5f);
    draw_list->AddCircleFilled(knob_center, switch_knob_radius, knob_color);
    ImU32 text_color = IM_COL32(0, 0, 0, 255);
    draw_list->AddText(label_pos, text_color, label);
    return pressed;
}
bool M_CircularCheckbox1(const char* label, bool* v) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    // float total_width = 210.0f;  
    // 替换：float total_width = 315.0f;
float total_width = ImGui::GetContentRegionAvail().x; // 直接使用全部可用宽度
    float total_height = 84.0f;  
    float switch_width = 85.0f;      
    float switch_height = 42.0f;     
    float switch_knob_radius = 15.0f; 
    float horizontal_margin = 20.0f;
    float vertical_padding = 15.0f;
    float spacing = 15.0f;
    ImRect total_bb(pos, ImVec2(pos.x + total_width, pos.y + total_height));
    ImGui::ItemSize(total_bb, vertical_padding);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImU32 background_color = IM_COL32(245, 245, 245, 200);
    float corner_radius = 12.0f;
    draw_list->AddRectFilled(total_bb.Min, total_bb.Max, background_color, corner_radius);
    ImU32 border_color = IM_COL32(200, 200, 200, 255);
    draw_list->AddRect(total_bb.Min, total_bb.Max, border_color, corner_radius, 0, 1.5f);
    if (hovered) {
        draw_list->AddRect(total_bb.Min, total_bb.Max, IM_COL32(150, 150, 150, 255), corner_radius, 0, 1.5f);
    }
    ImVec2 switch_pos = ImVec2(pos.x + total_width - horizontal_margin - switch_width,pos.y + (total_height - switch_height) * 0.5f);
    ImRect switch_bb(switch_pos, ImVec2(switch_pos.x + switch_width, switch_pos.y + switch_height));
    ImVec2 label_pos = ImVec2(pos.x + horizontal_margin,pos.y + (total_height - label_size.y) * 0.5f);
    ImU32 track_color, knob_color;
    if (*v) {
        track_color = IM_COL32(135, 190, 235, 255); // 修改后的激活轨迹颜色
        knob_color = IM_COL32(75, 190, 255, 255);  // 修改后的激活旋钮颜色
    } else {
        track_color = IM_COL32(180, 180, 180, 255);
        knob_color = IM_COL32(220, 220, 220, 255);
    }
    float track_rounding = switch_height * 0.5f;
    draw_list->AddRectFilled(switch_bb.Min, switch_bb.Max, track_color, track_rounding);
    float knob_x;
    if (*v) {
        knob_x = switch_bb.Max.x - switch_knob_radius - 6.0f; 
    } else {
        knob_x = switch_bb.Min.x + switch_knob_radius + 6.0f; 
    }
    ImVec2 knob_center = ImVec2(knob_x, (switch_bb.Min.y + switch_bb.Max.y) * 0.5f);
    draw_list->AddCircleFilled(knob_center, switch_knob_radius, knob_color);
    ImU32 text_color = IM_COL32(0, 0, 0, 255);
    draw_list->AddText(label_pos, text_color, label);
    return pressed;
}
float ImLerp(float a, float b, float t) { 
    return a + (b - a) * t; 
}
void CenteredText(const char* text, float customScale = 1.0f) {
    ImGui::SetWindowFontScale(customScale);
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
    ImGui::Text("%s", text);
    ImGui::SetWindowFontScale(1.0f);
}
// 预设颜色结构
struct 预设颜色 {
    const char* 名称;
    ImVec4 颜色值;
    ImU32 颜色值32;
};

// 预设颜色数组
static std::vector<预设颜色> 预设调色球 = {
    {"紫罗兰", ImVec4(0.54f, 0.17f, 0.89f, 1.0f), IM_COL32(138, 43, 226, 255)},
    {"克莱因蓝", ImVec4(0.0f, 0.18f, 0.65f, 1.0f), IM_COL32(0, 47, 167, 255)},
    {"爱马仕橙", ImVec4(0.96f, 0.33f, 0.09f, 1.0f), IM_COL32(245, 85, 23, 255)},
    {"蒂芙尼蓝", ImVec4(0.51f, 0.85f, 0.82f, 1.0f), IM_COL32(129, 216, 208, 255)},
    {"马尔斯绿", ImVec4(0.0f, 0.27f, 0.23f, 1.0f), IM_COL32(0, 69, 59, 255)},
    {"勃艮第红", ImVec4(0.51f, 0.0f, 0.13f, 1.0f), IM_COL32(130, 0, 33, 255)},
    {"普鲁士蓝", ImVec4(0.0f, 0.19f, 0.33f, 1.0f), IM_COL32(0, 49, 83, 255)},
    {"提香红", ImVec4(0.75f, 0.22f, 0.17f, 1.0f), IM_COL32(191, 56, 44, 255)},
    {"凡戴克棕", ImVec4(0.37f, 0.26f, 0.13f, 1.0f), IM_COL32(94, 66, 33, 255)},
    {"申布伦黄", ImVec4(0.93f, 0.78f, 0.0f, 1.0f), IM_COL32(237, 199, 0, 255)},
    {"松绿", ImVec4(0.18f, 0.36f, 0.34f, 1.0f), IM_COL32(46, 92, 87, 255)},
    {"玉色", ImVec4(0.62f, 0.85f, 0.71f, 1.0f), IM_COL32(158, 217, 181, 255)},
    {"海红", ImVec4(0.91f, 0.29f, 0.42f, 1.0f), IM_COL32(232, 74, 107, 255)},
    {"箬竹", ImVec4(0.47f, 0.56f, 0.42f, 1.0f), IM_COL32(120, 143, 107, 255)},
    {"柑宇", ImVec4(0.98f, 0.55f, 0.19f, 1.0f), IM_COL32(250, 140, 49, 255)},
    {"烟墨色", ImVec4(0.16f, 0.14f, 0.13f, 1.0f), IM_COL32(41, 36, 33, 255)},
    {"青雀头黛色", ImVec4(0.27f, 0.42f, 0.51f, 1.0f), IM_COL32(69, 107, 130, 255)},
    {"浅松绿", ImVec4(0.67f, 0.83f, 0.74f, 1.0f), IM_COL32(171, 212, 189, 255)},
    {"东方既白", ImVec4(0.85f, 0.92f, 0.96f, 1.0f), IM_COL32(217, 235, 245, 255)},
    {"浅云", ImVec4(0.93f, 0.94f, 0.95f, 1.0f), IM_COL32(237, 240, 242, 255)},
    {"栀子", ImVec4(0.98f, 0.91f, 0.71f, 1.0f), IM_COL32(250, 232, 181, 255)},
    {"金黄", ImVec4(1.0f, 0.84f, 0.0f, 1.0f), IM_COL32(255, 215, 0, 255)},
    {"桔梗", ImVec4(0.6f, 0.4f, 0.8f, 1.0f), IM_COL32(153, 102, 204, 255)},
    {"桃天", ImVec4(0.98f, 0.75f, 0.85f, 1.0f), IM_COL32(250, 191, 217, 255)},
    {"鹤顶红", ImVec4(0.9f, 0.2f, 0.2f, 1.0f), IM_COL32(230, 51, 51, 255)},
    {"乌黑", ImVec4(0.2f, 0.2f, 0.2f, 1.0f), IM_COL32(51, 51, 51, 255)},
    {"茶白", ImVec4(0.96f, 0.96f, 0.92f, 1.0f), IM_COL32(245, 245, 235, 255)},
    {"番茄红", ImVec4(1.0f, 0.39f, 0.28f, 1.0f), IM_COL32(255, 99, 71, 255)},
    {"藕白", ImVec4(0.98f, 0.92f, 0.84f, 1.0f), IM_COL32(250, 235, 215, 255)},
    {"烟雾蓝", ImVec4(0.55f, 0.78f, 0.94f, 1.0f), IM_COL32(140, 200, 240, 255)},
    {"奶酪黄", ImVec4(1.0f, 0.94f, 0.71f, 1.0f), IM_COL32(255, 240, 181, 255)},
    {"棕红", ImVec4(0.65f, 0.16f, 0.16f, 1.0f), IM_COL32(166, 41, 41, 255)},
    {"孔雀蓝", ImVec4(0.0f, 0.6f, 0.73f, 1.0f), IM_COL32(0, 153, 186, 255)},
    {"靛青", ImVec4(0.0f, 0.5f, 0.5f, 1.0f), IM_COL32(0, 128, 128, 255)},
    {"卡其", ImVec4(0.76f, 0.69f, 0.57f, 1.0f), IM_COL32(195, 176, 145, 255)},
    {"梅子青", ImVec4(0.47f, 0.78f, 0.65f, 1.0f), IM_COL32(120, 200, 166, 255)},
    {"透黄", ImVec4(1.0f, 0.94f, 0.35f, 1.0f), IM_COL32(255, 240, 89, 255)},
    {"珊瑚", ImVec4(1.0f, 0.5f, 0.31f, 1.0f), IM_COL32(255, 127, 80, 255)},
    {"纯白", ImVec4(1.0f, 1.0f, 1.0f, 1.0f), IM_COL32(255, 255, 255, 255)},
    {"纯黑", ImVec4(0.0f, 0.0f, 0.0f, 1.0f), IM_COL32(0, 0, 0, 255)}
};

static int 当前预设颜色索引 = 0;
static bool 显示预设调色板 = false;
bool 币子の调色器(const char* label, float color[4], int 颜色配置类型, bool 显示预设) {
    bool 已修改 = false;
    ImGui::PushID(label);
    static std::map<std::string, bool> 各标签显示状态;
    static std::map<std::string, int> 各标签选中索引;
    static std::map<std::string, float> 各标签动画进度;
    static std::map<std::string, float> 各标签目标高度;
    static std::map<std::string, float> 各标签当前高度;
    static std::map<std::string, float> 各标签滑入动画; 
    if (各标签显示状态.find(label) == 各标签显示状态.end()) {
        各标签显示状态[label] = false;
    }
    if (各标签选中索引.find(label) == 各标签选中索引.end()) {
        各标签选中索引[label] = -1;
    }
    if (各标签动画进度.find(label) == 各标签动画进度.end()) {
        各标签动画进度[label] = 0.0f;
    }
    if (各标签目标高度.find(label) == 各标签目标高度.end()) {
        各标签目标高度[label] = 0.0f;
    }
    if (各标签当前高度.find(label) == 各标签当前高度.end()) {
        各标签当前高度[label] = 0.0f;
    }
    if (各标签滑入动画.find(label) == 各标签滑入动画.end()) {
        各标签滑入动画[label] = 0.0f; 
    }
    bool& 当前标签显示状态 = 各标签显示状态[label];
    int& 当前标签选中索引 = 各标签选中索引[label];
    float& 当前标签动画进度 = 各标签动画进度[label];
    float& 当前标签目标高度 = 各标签目标高度[label];
    float& 当前标签当前高度 = 各标签当前高度[label];
    float& 当前标签滑入进度 = 各标签滑入动画[label]; 
    float 目标进度 = 当前标签显示状态 ? 1.0f : 0.0f;
    float 动画速度 = 8.0f;
    if (fabs(当前标签动画进度 - 目标进度) > 0.001f) {
        Lerp(当前标签动画进度, 目标进度, ImGui::GetIO().DeltaTime * 动画速度);
    } else {
        当前标签动画进度 = 目标进度;
    }
    float 目标滑入进度 = 当前标签显示状态 ? 1.0f : 0.0f;
    if (fabs(当前标签滑入进度 - 目标滑入进度) > 0.001f) {
        Lerp(当前标签滑入进度, 目标滑入进度, ImGui::GetIO().DeltaTime * (动画速度 * 0.8f));
    } else {
        当前标签滑入进度 = 目标滑入进度;
    }
    float 动画系数 = 0.0f;
    if (当前标签动画进度 <= 0.5f) {
        动画系数 = 2.0f * 当前标签动画进度 * 当前标签动画进度;
    } else {
        动画系数 = 1.0f - 2.0f * (1.0f - 当前标签动画进度) * (1.0f - 当前标签动画进度);
    }
    float 滑入系数 = 0.0f;
    if (当前标签滑入进度 <= 0.5f) {
        滑入系数 = 2.0f * 当前标签滑入进度 * 当前标签滑入进度;
    } else {
        滑入系数 = 1.0f - 2.0f * (1.0f - 当前标签滑入进度) * (1.0f - 当前标签滑入进度);
    }
    ImGui::BeginGroup();
    float 信息区域偏移 = 50.0f * (1.0f - 滑入系数);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 信息区域偏移);
    ImGui::Text("%s", label);
    if (当前标签选中索引 >= 0 && 当前标签选中索引 < 预设调色球.size()) {
        static std::map<std::string, float> 文字动画进度;
        if (文字动画进度.find(label) == 文字动画进度.end()) {
            文字动画进度[label] = 0.0f;
        }
        float& 文字进度 = 文字动画进度[label];
        if (fabs(文字进度 - 滑入系数) > 0.01f) {
            Lerp(文字进度, 滑入系数, ImGui::GetIO().DeltaTime * 10.0f);
        } else {
            文字进度 = 滑入系数;
        }
        std::string 当前文本 = "当前: " + std::string(预设调色球[当前标签选中索引].名称);
        int 显示字符数 = (int)(当前文本.length() * 文字进度);
        ImGui::Text("%s", 当前文本.substr(0, 显示字符数).c_str());
    } else {
        static std::map<std::string, float> 自定义文字动画;
        if (自定义文字动画.find(label) == 自定义文字动画.end()) {
            自定义文字动画[label] = 0.0f;
        }
        float& 文字进度 = 自定义文字动画[label];
        if (fabs(文字进度 - 滑入系数) > 0.01f) {
            Lerp(文字进度, 滑入系数, ImGui::GetIO().DeltaTime * 10.0f);
        } else {
            文字进度 = 滑入系数;
        }
        std::string 当前文本 = "当前: 自定义";
        int 显示字符数 = (int)(当前文本.length() * 文字进度);
        ImGui::Text("%s", 当前文本.substr(0, 显示字符数).c_str());
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::PushItemWidth(80); 
    float 选择器缩放 = 0.8f + 0.2f * 滑入系数;
    ImGui::SetWindowFontScale(选择器缩放);
    if (ImGui::ColorEdit4("##颜色选择器", color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip)) {
        已修改 = true;
        当前标签选中索引 = -1;
        for (int i = 0; i < 预设调色球.size(); i++) {
            if (fabs(color[0] - 预设调色球[i].颜色值.x) < 0.01f &&fabs(color[1] - 预设调色球[i].颜色值.y) < 0.01f &&
                fabs(color[2] - 预设调色球[i].颜色值.z) < 0.01f &&fabs(color[3] - 预设调色球[i].颜色值.w) < 0.01f) {
                当前标签选中索引 = i;
                break;
            }
        }
    }
    ImGui::SetWindowFontScale(1.0f); 
    ImGui::PopItemWidth();
    if (显示预设) {
        ImGui::SameLine();
        const char* 按钮文字 = 当前标签显示状态 ? "隐藏预设" : "显示预设";
        float 按钮浮动偏移 = sinf(ImGui::GetTime() * 3.0f) * 2.0f * 滑入系数;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 按钮浮动偏移);
        ImVec4 原始按钮颜色 = ImGui::GetStyle().Colors[ImGuiCol_Button];
        ImVec4 目标按钮颜色 = 原始按钮颜色;
        if (当前标签动画进度 > 0.5f) {
            目标按钮颜色.x *= 1.1f;
            目标按钮颜色.y *= 1.1f;
            目标按钮颜色.z *= 1.1f;
        } else {
            目标按钮颜色.x *= 0.9f;
            目标按钮颜色.y *= 0.9f;
            目标按钮颜色.z *= 0.9f;
        }
        ImVec4 当前按钮颜色;
        Lerp(当前按钮颜色.x, 原始按钮颜色.x, 目标按钮颜色.x, 动画系数);
        Lerp(当前按钮颜色.y, 原始按钮颜色.y, 目标按钮颜色.y, 动画系数);
        Lerp(当前按钮颜色.z, 原始按钮颜色.z, 目标按钮颜色.z, 动画系数);
        当前按钮颜色.w = 原始按钮颜色.w;
        ImGui::PushStyleColor(ImGuiCol_Button, 当前按钮颜色);
        if (ImGui::Button(按钮文字)) {
            当前标签显示状态 = !当前标签显示状态;
        }
        ImGui::PopStyleColor();
    }
    if (显示预设 && 当前标签滑入进度 > 0.01f) {
        ImGui::Separator();
        float 滑入偏移量 = 300.0f * (1.0f - 滑入系数); 
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 滑入偏移量);
        int 总颜色数量 = 预设调色球.size();
        int 每行数量 = 5;
        int 行数 = (总颜色数量 + 每行数量 - 1) / 每行数量;
        float 颜色按钮大小 = 93.0f;
        float 水平间隔 = 35.0f;
        float 垂直间隔 = 40.0f;
        float 文本区域高度 = 25.0f;
        float 计算目标高度 = 行数 * (颜色按钮大小 + 文本区域高度 + 垂直间隔) + 50.0f;
        计算目标高度 = ImClamp(计算目标高度, 200.0f, 1200.0f);
        if (fabs(当前标签目标高度 - 计算目标高度) > 0.1f) {
            Lerp(当前标签目标高度, 计算目标高度, 0.1f);
        }
        if (fabs(当前标签当前高度 - 当前标签目标高度 * 滑入系数) > 0.1f) {
            Lerp(当前标签当前高度, 当前标签目标高度 * 滑入系数, 0.15f);
        } else {
            当前标签当前高度 = 当前标签目标高度 * 滑入系数;
        }
        ImGui::BeginChild("##预设调色板", ImVec2(0, 当前标签当前高度), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        float 标题偏移 = 20.0f * (1.0f - 滑入系数);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 标题偏移);
        ImGui::Text("%s 颜色预设:", label);
        float 实际总宽度 = 每行数量 * 颜色按钮大小 + (每行数量 - 1) * 水平间隔;
        float 起始偏移 = (ImGui::GetContentRegionAvail().x - 实际总宽度) * 0.5f;
        if (起始偏移 > 0) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 起始偏移);
        }
        for (int i = 0; i < 预设调色球.size(); i++) {
            float 块延迟 = i * 0.02f; 
            float 块动画进度 = ImClamp((滑入系数 - 块延迟) / (1.0f - 块延迟), 0.0f, 1.0f);
            if (块动画进度 > 0.0f) {
                float 块缩放 = 0.5f + 0.5f * 块动画进度;
                if (i % 每行数量 != 0) {
                    ImGui::SameLine(0, 水平间隔);
                } else if (i > 0) {
                    ImGui::Spacing();
                    ImGui::Spacing();
                    if (起始偏移 > 0) {
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 起始偏移);
                    }
                }
                ImGui::BeginGroup();
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.5f);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                bool 当前选中 = (当前标签选中索引 == i);
                if (当前选中) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(预设调色球[i].颜色值.x * 0.7f,预设调色球[i].颜色值.y * 0.7f,预设调色球[i].颜色值.z * 0.7f,预设调色球[i].颜色值.w));
                } else {
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
                    ImGui::PushStyleColor(ImGuiCol_Button, 预设调色球[i].颜色值);
                }
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(预设调色球[i].颜色值.x * 1.2f, 预设调色球[i].颜色值.y * 1.2f, 预设调色球[i].颜色值.z * 1.2f, 预设调色球[i].颜色值.w));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(预设调色球[i].颜色值.x * 0.8f, 预设调色球[i].颜色值.y * 0.8f, 预设调色球[i].颜色值.z * 0.8f, 预设调色球[i].颜色值.w));
                ImVec2 按钮尺寸 = ImVec2(颜色按钮大小 * 块缩放, 颜色按钮大小 * 块缩放);
                ImGui::PushID(i);
                if (ImGui::Button("##颜色按钮", 按钮尺寸)) {
                    color[0] = 预设调色球[i].颜色值.x;
                    color[1] = 预设调色球[i].颜色值.y;
                    color[2] = 预设调色球[i].颜色值.z;
                    color[3] = 预设调色球[i].颜色值.w;
                    当前标签选中索引 = i;
                    已修改 = true;
                }
                if (当前选中) {
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    ImVec2 按钮位置 = ImGui::GetItemRectMin();
                    ImVec2 按钮实际尺寸 = ImGui::GetItemRectSize();
                    float 边框大小 = 3.0f * 块缩放;
                    float 边框扩展 = 边框大小;
                    ImVec2 边框开始位置 = ImVec2(按钮位置.x - 边框扩展, 按钮位置.y - 边框扩展);
                    ImVec2 边框结束位置 = ImVec2(按钮位置.x + 按钮实际尺寸.x + 边框扩展, 按钮位置.y + 按钮实际尺寸.y + 边框扩展);
                    float 边框圆角 = 25.5f + 边框扩展 * 0.5f;
                    draw_list->AddRect(边框开始位置, 边框结束位置, IM_COL32(0, 120, 255, (int)(255 * 块动画进度)), 边框圆角, ImDrawFlags_RoundCornersAll, 8.0f);
                }
                ImGui::PopID();
                if (当前选中) {
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(1);
                } else {
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(1);
                }
                ImGui::PopStyleVar(2);
                ImVec2 当前光标位置 = ImGui::GetCursorPos();
                float 文本宽度 = ImGui::CalcTextSize(预设调色球[i].名称).x;
                float 最大允许宽度 = 颜色按钮大小;
                const char* 显示文本 = 预设调色球[i].名称;
                if (文本宽度 > 最大允许宽度) {
                    float 缩放比例 = 最大允许宽度 / 文本宽度;
                    ImGui::SetWindowFontScale(缩放比例 * 0.9f);
                    文本宽度 = ImGui::CalcTextSize(显示文本).x;
                }
                float 水平偏移量 = (颜色按钮大小 - 文本宽度) * 0.5f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImMax(0.0f, 水平偏移量));
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 块动画进度);
                ImGui::Text("%s", 显示文本);
                ImGui::PopStyleVar();
                if (文本宽度 > 最大允许宽度) {
                    ImGui::SetWindowFontScale(1.0f);
                }
                ImGui::SetCursorPosY(当前光标位置.y + 文本区域高度);
                ImGui::EndGroup();
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup(); 
        ImGui::Separator();
    }
    ImGui::PopID();
    return 已修改;
}