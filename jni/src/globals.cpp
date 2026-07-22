#include "globals.h"
namespace image {
    ImTextureID background_preview = nullptr;
    ImTextureID preview_model = nullptr;
    ImTextureID logo = nullptr;
    ImTextureID logoo = nullptr;
}
namespace font {
    ImFont* icomoon = nullptr;
    ImFont* icomoon_tabs = nullptr;
    ImFont* icomoon_subtab = nullptr;
    ImFont* icomoon_widget = nullptr;
    ImFont* inter_child = nullptr;
    ImFont* inter_element = nullptr;
}
namespace esp_preview {
    bool money = true;
    bool nickname = true;
    bool weapon = true;
    bool zoom = true;
    bool c4 = true;
    bool HP_line = true;
    bool hit = true;
    bool box = true;
    bool bomb = true;
    float box_color[4] = { 37 / 255.f, 37 / 255.f, 47 / 255.f, 1.f };
    float nick_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float money_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float zoom_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float c4_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float bomb_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float hp_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float hp_line_color[4] = { 112 / 255.f, 109 / 255.f, 214 / 255.f, 1.f };
    float weapon_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    float hit_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    int hp = 85;
}
bool info_bar = true;
const char* cheat_name = "RAGNAREK";
const char* game_status = "Counter-Strike: 2";
const char* developer = "Past Owl";
const char* pingi = "45ms";
const char* world_time = "12:20am";
float tab_alpha = 0.f; 
float tab_add; 
int active_tab = 0;
ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_DisplayHex;
