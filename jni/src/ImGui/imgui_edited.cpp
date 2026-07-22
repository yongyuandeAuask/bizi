#include "imgui_edited.hpp"
#include <vector>
#include <chrono>
#include <map>
using namespace ImGui;

namespace edited
{
    // 修复颜色常量定义
namespace c {
    namespace text {
        static const ImVec4 text = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);        // 黑色（不按下）
        static const ImVec4 text_hov = ImVec4(0.3f, 0.0f, 0.0f, 1.0f);     // 深红色（悬停）
        static const ImVec4 text_active = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 红色（激活）
    }
    namespace widget {
        static const ImVec4 background = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);  // 浅灰色背景确保黑色文字可见
        static const ImVec4 selectable = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        static const float rounding = 5.0f;
    }
    static const ImVec4 accent_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);   // 红色强调色
    static const ImVec4 while_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}


    // 辅助函数：带alpha的GetColorU32
    ImU32 GetColorU32(const ImVec4& col, float alpha_multiplier = 1.0f) {
        return ImGui::GetColorU32(ImVec4(col.x, col.y, col.z, col.w * alpha_multiplier));
    }

    // Checkbox相关的状态结构
    struct check_state
    {
        ImVec4 background, circle, text;
        float background_opticaly, circle_offset;
    };

    bool Checkbox(const char* label, bool* v)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        const float square_sz = 38, y_size = 38;
        const ImVec2 pos = window->DC.CursorPos;
        const float w = GetContentRegionMax().x - style.WindowPadding.x;
        const ImRect total_bb(pos + ImVec2(0, 0), pos + ImVec2(w, y_size));
        
        ItemSize(total_bb, 0.f);
        if (!ItemAdd(total_bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);

        static std::map<ImGuiID, check_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, check_state() });
            it_anim = anim.find(id);
        }

        it_anim->second.background_opticaly = ImLerp(it_anim->second.background_opticaly, *v ? 0.2f : 1.0f, g.IO.DeltaTime * 6.f);
        it_anim->second.circle_offset = ImLerp(it_anim->second.circle_offset, *v ? 0 : -square_sz * 2, g.IO.DeltaTime * 6.f);
        it_anim->second.background = ImLerp(it_anim->second.background, *v ? c::accent_color : c::widget::background, g.IO.DeltaTime * 6.f);
        it_anim->second.circle = ImLerp(it_anim->second.circle, *v ? c::accent_color : c::text::text, g.IO.DeltaTime * 6.f);
        it_anim->second.text = ImLerp(it_anim->second.text, *v ? c::text::text_active : hovered ? c::text::text_hov : c::text::text, g.IO.DeltaTime * 6.f);

        if (pressed)
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        const ImRect check_bb(pos + ImVec2(w - (square_sz * 2), (y_size - square_sz) / 2), pos + ImVec2(w, (y_size + square_sz) / 2));

        // 修复GetColorU32调用
        window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.background, it_anim->second.background_opticaly), 30);
        window->DrawList->AddRect(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.background), 30, NULL, 1.1f);

        window->DrawList->AddCircleFilled(check_bb.Max - ImVec2(y_size - it_anim->second.circle_offset, y_size) / 2, 8.f, GetColorU32(it_anim->second.circle), 50);
        window->DrawList->AddText(pos + ImVec2(0, (y_size - CalcTextSize(label).y) / 2), GetColorU32(it_anim->second.text), label);

        return pressed;
    }

    // Slider相关的状态结构
    struct slider_state
    {
        ImVec4 text, circle;
        float slow, hint_opticaly, hint_alpha, circle_pump;
    };

    // 辅助函数：文本居中显示
    void TextCenter(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::RenderTextClipped(p_min, p_max, text, NULL, NULL, align, NULL);
        ImGui::PopStyleColor();
    }

    bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = GetContentRegionMax().x - style.WindowPadding.x;
        const float y_size = 18;
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, y_size));
        const ImRect slider_bb(window->DC.CursorPos + ImVec2(160, (y_size / 2) - 1), window->DC.CursorPos + ImVec2(w - 80, (y_size / 2) + 1));
        const ImRect slider_active(window->DC.CursorPos + ImVec2(160, 0), window->DC.CursorPos + ImVec2(w - 80, y_size));

        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x, 0));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

        ItemSize(ImRect(total_bb.Min, total_bb.Max));
        if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

        if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

        // 修复：添加正确的ItemHoverable参数
        bool hovered = ItemHoverable(slider_active, id, 0), held, pressed = ButtonBehavior(slider_active, id, &hovered, &held, NULL);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);

        if (temp_input_is_active)
        {
            const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
            return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
        }

        static std::map<ImGuiID, slider_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, slider_state() });
            it_anim = anim.find(id);
        }

        it_anim->second.hint_opticaly = ImLerp(it_anim->second.hint_opticaly, hovered ? 0.3f : 0.0f, g.IO.DeltaTime * 6.f);
        it_anim->second.hint_alpha = ImLerp(it_anim->second.hint_alpha, hovered || IsItemActive() ? 1.f : 0.f, g.IO.DeltaTime * 6.f);
        it_anim->second.text = ImLerp(it_anim->second.text, IsItemActive() ? c::text::text_active : hovered ? c::text::text_hov : c::text::text, g.IO.DeltaTime * 6.f);

        window->DrawList->AddRectFilled(slider_bb.Min + ImVec2(0, 0), slider_bb.Max + ImVec2(0, 1), GetColorU32(c::widget::background), 30.f);

        ImRect grab_bb;
        const bool value_changed = SliderBehavior(ImRect(slider_bb.Min + ImVec2(0, 0), slider_bb.Max + ImVec2(7, 0)), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
        
        if (value_changed) MarkItemEdited(id);

        if (grab_bb.Max.x > grab_bb.Min.x) {
            it_anim->second.slow = ImLerp(it_anim->second.slow, grab_bb.Min.x - frame_bb.Min.x, g.IO.DeltaTime * 25.f);

            PushClipRect(slider_bb.Min - ImVec2(0, 5), slider_bb.Max + ImVec2(0, 5), true);
            window->DrawList->AddRectFilled(slider_bb.Min + ImVec2(0, 1), ImVec2(it_anim->second.slow + frame_bb.Min.x - 3 - it_anim->second.circle_pump, grab_bb.Max.y + 10), GetColorU32(c::accent_color), 30.f);
            PopClipRect();

            window->DrawList->AddCircleFilled(ImVec2(frame_bb.Min.x + it_anim->second.slow, grab_bb.Max.y + 5), 11.5f, GetColorU32(c::accent_color), 30.f);
        }

        window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(w - 58, -12), frame_bb.Max + ImVec2(0, 1), GetColorU32(c::widget::background), 4.f);
        TextCenter(frame_bb.Min + ImVec2(w - 58, -12), frame_bb.Max - ImVec2(0, 1), GetColorU32(c::text::text_active), value_buf, ImVec2(0.5, 0.5));
        window->DrawList->AddText(ImVec2(frame_bb.Max.x - w, frame_bb.Max.y - ((y_size + CalcTextSize(label).y) / 2)), GetColorU32(it_anim->second.text), label);

        return value_changed;
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
    }

    // Button相关的状态结构
    struct button_state {
        ImVec4 background, text;
        float offset_y, bg_alpha;
    };
    
    bool Button(const char* label, const ImVec2& size_arg) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;
    
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
        
        const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id)) return false;
    
        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);
    
        static std::map<ImGuiID, button_state> anim;
        auto it_anim = anim.find(id);
        if (it_anim == anim.end()) {
            anim.insert({id, button_state()});
            it_anim = anim.find(id);
        }
    
        // 状态插值
        it_anim->second.background = ImLerp(it_anim->second.background, 
            held ? c::accent_color : hovered ? c::widget::selectable : c::widget::background, 
            g.IO.DeltaTime * 12.f);
        
        it_anim->second.text = ImLerp(it_anim->second.text,
            held ? c::while_color : hovered ? c::text::text_active : c::text::text,
            g.IO.DeltaTime * 12.f);
        
        it_anim->second.offset_y = ImLerp(it_anim->second.offset_y, 
            held ? 2.0f : 0.0f, 
            g.IO.DeltaTime * 15.f);
        
        it_anim->second.bg_alpha = ImLerp(it_anim->second.bg_alpha,
            hovered ? 1.1f : 1.0f,
            g.IO.DeltaTime * 10.f);
    
        // 绘制背景 - 修复DrawList调用
        window->DrawList->AddRectFilled(
            bb.Min + ImVec2(0, it_anim->second.offset_y),
            bb.Max + ImVec2(0, it_anim->second.offset_y),
            GetColorU32(it_anim->second.background, it_anim->second.bg_alpha),
            c::widget::rounding);
    
        // 绘制边框 - 修复DrawList调用
        if (hovered || held) {
            window->DrawList->AddRect(
                bb.Min + ImVec2(0, it_anim->second.offset_y),
                bb.Max + ImVec2(0, it_anim->second.offset_y),
                GetColorU32(c::accent_color, 0.4f),
                c::widget::rounding,
                0, 1.5f);
        }
    
        // 绘制文字
        const ImVec2 text_pos = bb.Min + 
            (size - label_size) * 0.5f + 
            ImVec2(0, it_anim->second.offset_y);
        
        PushStyleColor(ImGuiCol_Text, GetColorU32(it_anim->second.text));
        RenderText(text_pos, label);
        PopStyleColor();
    
        // 点击效果 - 修复DrawList调用
        if (pressed) {
            const float t = ImMin(g.IO.DeltaTime * 50, 1.0f);
            window->DrawList->AddCircleFilled(
                bb.GetCenter() + ImVec2(0, it_anim->second.offset_y),
                label_size.x * 0.6f * (1.0f - t),
                GetColorU32(ImVec4(1,1,1,0.15f * t)),
                16);
        }
    
        return pressed;
    }
}
