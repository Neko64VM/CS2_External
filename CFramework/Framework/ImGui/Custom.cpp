#include "Custom.h"
#include <unordered_map>

// ToDo : 全体的に書き直す

void ImGui::RenderCustomFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, ImU32 sub_col, bool border, float rounding)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
    window->DrawList->AddRectFilled(ImVec2(p_min.x, p_min.y), ImVec2(p_min.x + 3.f, p_max.y), sub_col, 0.f);

    const float border_size = g.Style.FrameBorderSize;
    if (border && border_size > 0.0f)
    {
        window->DrawList->AddRect(ImVec2(p_min.x + 1.f, p_min.y + 1.f), ImVec2(p_max.x + 1.f, p_max.y + 1.f), GetColorU32(ImGuiCol_BorderShadow), rounding, 0, border_size);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, 0, border_size);
    }
}

bool ImGui::CustomButton(const char* icon, const char* label, const ImVec2& size_arg, const bool& flag)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(icon);
    const ImVec2 label_size = CalcTextSize(icon, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x, label_size.y + style.FramePadding.y);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_None);
    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    // --- フェード状態の保持 ---
    static std::unordered_map<ImGuiID, float> fade_map;
    float& fade = fade_map[id];
    float target = flag ? 1.0f : 0.0f;
    float speed = 10.5f * g.IO.DeltaTime; // 調整可（数字が大きいほど速くフェード）

    fade = ImClamp(fade + (target - fade) * speed, 0.0f, 1.0f);

    // sub_col の alpha をフェード値に応じて調整
    ImU32 base_col = GetColorU32(ImGuiCol_CheckMark); // base color
    ImVec4 col_sub = ColorConvertU32ToFloat4(base_col);
    col_sub.w *= fade; // alpha を調整
    ImU32 sub = ColorConvertFloat4ToU32(col_sub);

    RenderNavHighlight(bb, id);
    RenderCustomFrame(bb.Min, bb.Max, col, sub, true, style.FrameRounding);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");

    RenderTextClipped(ImVec2(bb.Min.x + style.FramePadding.x + 10, bb.Min.y), ImVec2(bb.Min.x - style.FramePadding.x - 10, bb.Max.y), icon, NULL, &label_size, style.ButtonTextAlign, &bb); // Icon
    RenderTextClipped(ImVec2(bb.Min.x - 20, bb.Min.y), ImVec2(bb.Max.x - 20, bb.Max.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);  // Text

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}