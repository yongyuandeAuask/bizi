#pragma once
#include <imgui.h>
namespace image {
    extern ImTextureID background_preview;
    extern ImTextureID preview_model;
    extern ImTextureID logo;
    extern ImTextureID logoo;
}
namespace font {
    extern ImFont* icomoon;
    extern ImFont* icomoon_tabs;
    extern ImFont* icomoon_subtab;
    extern ImFont* icomoon_widget;
    extern ImFont* inter_child;
    extern ImFont* inter_element;
}
namespace esp_preview {
    extern bool money;
    extern bool nickname;
    extern bool weapon;
    extern bool zoom;
    extern bool c4;
    extern bool HP_line;
    extern bool hit;
    extern bool box;
    extern bool bomb;
    extern float box_color[4];
    extern float nick_color[4];
    extern float money_color[4];
    extern float zoom_color[4];
    extern float c4_color[4];
    extern float bomb_color[4];
    extern float hp_color[4];
    extern float hp_line_color[4];
    extern float weapon_color[4];
    extern float hit_color[4];
    extern int hp;
}
extern bool info_bar;
extern const char* cheat_name;
extern const char* game_status;
extern const char* developer;
extern const char* pingi;
extern const char* world_time;
extern float tab_alpha;
extern float tab_add;
extern int active_tab;
extern ImGuiColorEditFlags picker_flags;
