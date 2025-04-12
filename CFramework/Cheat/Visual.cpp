#include "CFramework.h"

CEntity lastTarget = CEntity();

void CFramework::RenderInfo()
{
    ImGui::SetNextWindowPos(ImVec2(g.ptPoint.x, g.ptPoint.y));
    ImGui::SetNextWindowSize(ImVec2(g.rcSize.right, g.rcSize.bottom));
    ImGui::Begin("##Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);

    // FPS
    String(Vector2(3.f, 3.f), TEXT_COLOR, 1.f, std::to_string((int)ImGui::GetIO().Framerate).c_str());

    // FOV Circle
    if (g.AimBotEnable && g.bShowFOV)
        DrawCircle(Vector2((g.rcSize.right / 2.f), (g.rcSize.bottom / 2.f)), g.AimFOV, g.bRainbowFOV ? GenerateRainbow() : g.Color_AimFOV, 0.35f);

    // Crosshair
    if (g.CrosshairEnable)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.rcSize.right / 2, g.rcSize.bottom / 2);
            ImColor crosshair_col = WithAlpha(g.Color_Crosshair, g.m_flGlobalAlpha);

            DrawLine(Vector2(Center.x - g.CrosshairSize, Center.y), Vector2((Center.x + g.CrosshairSize) + 1, Center.y), crosshair_col, 1.f);
            DrawLine(Vector2(Center.x, Center.y - g.CrosshairSize), Vector2(Center.x, (Center.y + g.CrosshairSize) + 1), crosshair_col, 1.f);
        }   break;
        case 1:
            DrawCircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), 0.85f); // 0.85f == CrosshairAlpha
            DrawCircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize, g.Color_Crosshair, 0.85f);
            break;
        }
    }

    ImGui::End();
}

void CFramework::RenderESP()
{
    // AimBot
    float FOV = 0.f;
    float MinFov = FLT_MAX;
    float MinDistance = FLT_MAX;
    CEntity target = CEntity();
    Vector2 ScreenMiddle = { g.rcSize.right / 2.f, g.rcSize.bottom / 2.f };

    // Local
    CEntity local = CEntity();
    local.m_address = offset.GetLocal();

    // Localの更新に失敗したらこの関数を終了
    uintptr_t entitylist = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset.dwEntityList);
    if (!local.UpdateStatic(entitylist))

        return;
    if (!local.Update())
        return;

    // ViewMatrixとかいろいろ
    Matrix ViewMatrix = m.Read<Matrix>(m.m_dwClientBaseAddr + offset.dwViewMatrix);

    CEntity* pLocal = &local;

    for (auto& entity : this->GetEntityList())
    {
        CEntity* pEntity = &entity;

        if (!pEntity->Update())
            continue;

        std::vector<Vector3> BoneList;

        if (g.AimBotEnable || g.bSkeleton)
            BoneList = pEntity->GetBoneList();

        // 距離を取得
        const float flDistance = ((pLocal->m_vOldOrigin - pEntity->m_vOldOrigin).Length() * 0.01905f);

        // 各種チェック
        if (g.ESP_MaxDistance < flDistance)
            continue;
        else if (!g.ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        BoundingBox box = pEntity->GetBoundingBoxData(ViewMatrix);

        if (box.left == 0)
            continue;

        // サイズ算出
        const int Height = box.bottom - box.top;
        const int Width = box.right - box.left;
        const int Center = (box.right - box.left) / 2.f;
        const int bScale = (box.right - box.left) / 3.f;

        // 対象が見えてるかチェックする。
        bool visible = false;

        // 色を決める
        ImColor shadow_color = WithAlpha(g.Color_ESP_Shadow, g.m_flShadowAlpha);
        ImColor tempColor = pLocal->m_iTeamNum == pEntity->m_iTeamNum ? g.Color_ESP_Team : g.Color_ESP_Enemy;
       
        if (pEntity->m_address == lastTarget.m_address)
            tempColor = g.Color_ESP_AimTarget;

        ImColor color = WithAlpha(tempColor, g.m_flGlobalAlpha);

        // Line
        if (g.bLine)
            DrawLine(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom), Vector2(box.right - (Width / 2), box.bottom), color, g.m_flGlobalAlpha);

        // Box
        if (g.bBox)
        {
            // BoxFilled
            if (g.bFilled)
                RectFilled(box.left, box.top, box.right, box.bottom, shadow_color, g.m_flShadowAlpha);

            // Shadow
            DrawLine(Vector2(box.left - 1, box.top - 1), Vector2(box.right + 2, box.top - 1), shadow_color);
            DrawLine(Vector2(box.left - 1, box.top), Vector2(box.left - 1, box.bottom + 2), shadow_color);
            DrawLine(Vector2(box.right + 1, box.top), Vector2(box.right + 1, box.bottom + 2), shadow_color);
            DrawLine(Vector2(box.left - 1, box.bottom + 1), Vector2(box.right + 1, box.bottom + 1), shadow_color);

            switch (g.ESP_BoxType)
            {
            case 0:
                DrawBox(box.right, box.left, box.top, box.bottom, color);
                break;
            case 1:
                DrawLine(Vector2(box.left, box.top), Vector2(box.left + bScale, box.top), color); // Top
                DrawLine(Vector2(box.right, box.top), Vector2(box.right - bScale, box.top), color);
                DrawLine(Vector2(box.left, box.top), Vector2(box.left, box.top + bScale), color); // Left
                DrawLine(Vector2(box.left, box.bottom), Vector2(box.left, box.bottom - bScale), color);
                DrawLine(Vector2(box.right, box.top), Vector2(box.right, box.top + bScale), color); // Right
                DrawLine(Vector2(box.right, box.bottom), Vector2(box.right, box.bottom - bScale), color);
                DrawLine(Vector2(box.left, box.bottom), Vector2(box.left + bScale, box.bottom), color); // Bottom
                DrawLine(Vector2(box.right + 1, box.bottom), Vector2(box.right - bScale, box.bottom), color);
                break;
            }
        }

        // Skeleton
        if (g.bSkeleton)
        {
            if (BoneList.size() == 32) {
                // 頭の円をレンダリング
                Vector2 pHead, pNeck;
                if (!WorldToScreen(ViewMatrix, g.rcSize, BoneList[BONE_HEAD], pHead) ||
                    !WorldToScreen(ViewMatrix, g.rcSize, BoneList[BONE_NECK], pNeck))
                    continue;

                DrawCircle(pHead, pNeck.y - pHead.y, color, g.m_flGlobalAlpha);

                // 線を引くためのペアを作成する
                const Vector3 skeleton_list[][2] = {
                    { BoneList[BONE_NECK], BoneList[BONE_HIP] },
                    { BoneList[BONE_NECK], BoneList[BONE_LEFT_SHOULDER] },
                    { BoneList[BONE_LEFT_SHOULDER], BoneList[BONE_LEFT_ARM] },
                    { BoneList[BONE_LEFT_ARM], BoneList[BONE_LEFT_HAND] },
                    { BoneList[BONE_NECK], BoneList[BONE_RIGHT_SHOULDER] },
                    { BoneList[BONE_RIGHT_SHOULDER], BoneList[BONE_RIGHT_ARM] },
                    { BoneList[BONE_RIGHT_ARM], BoneList[BONE_RIGHT_HAND] },
                    { BoneList[BONE_HIP], BoneList[BONE_LEFT_KNEE] },
                    { BoneList[BONE_LEFT_KNEE], BoneList[BONE_LEFT_FEET] },
                    { BoneList[BONE_HIP], BoneList[BONE_RIGHT_KNEE] },
                    { BoneList[BONE_RIGHT_KNEE], BoneList[BONE_RIGHT_FEET] }
                };

                // WorldToScreenを行い各ペアをレンダリングする.
                for (int j = 0; j < 11; j++)
                {
                    Vector2 vOut0, vOut1;
                    if (!WorldToScreen(ViewMatrix, g.rcSize, skeleton_list[j][0], vOut0) ||
                        !WorldToScreen(ViewMatrix, g.rcSize, skeleton_list[j][1], vOut1))
                        break;

                    DrawLine(vOut0, vOut1, color);
                }
            }
        }

        // Healthbar
        if (g.bHealth)
            HealthBar(box.left - 3, box.bottom + 1, 1, -Height - 1, pEntity->m_iHealth, pEntity->m_iMaxHealth, shadow_color, g.m_flGlobalAlpha); // Health

        // Name
        if (g.bName)
            StringEx(Vector2(box.right - Center - (ImGui::CalcTextSize(pEntity->m_namePlayer).x / 2.f), box.top - ImGui::GetFontSize()), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), pEntity->m_namePlayer);

        // Distance & Weapon
        std::string outStr{};

        // Distance
        if (g.bDistance)
            outStr += "[ " + std::to_string((int)flDistance) + "m ]";

        // Weapon
        if (g.bWeapon)
            outStr += " " + pEntity->m_nameWeapon;

        // Rendering
        if (g.bDistance || g.bWeapon && outStr.size() > 0)
            StringEx(Vector2(box.right - Center - (ImGui::CalcTextSize(outStr.c_str()).x / 2.f), box.bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), outStr.c_str());

        if (flDistance > g.AimMaxDistance)
            continue;

        // AimBot
        // 多分FOV外の敵も部分的に狙ってるので要修正
        if (g.AimBotEnable && pLocal->m_iTeamNum != pEntity->m_iTeamNum)
        {
            for (const auto& bone : BoneList)
            {
                Vector2 BoneScreen{};
                if (!WorldToScreen(ViewMatrix, g.rcSize, bone, BoneScreen))
                    break;

                // In FOV?
                FOV = abs((ScreenMiddle - BoneScreen).Length());

                if (FOV < g.AimFOV)
                {
                    switch (g.AimMode)
                    {
                    case 0: // Crosshair
                        if (MinFov > FOV) {
                            if (target.m_address == NULL || MinDistance > flDistance)
                            {
                                target = entity;
                                MinFov = FOV;
                                MinDistance = flDistance;
                            }
                        }
                        break;
                    case 1: // Game Distance
                        if (MinDistance > flDistance) {
                            target = entity;
                            MinDistance = flDistance;
                        }
                        break;
                    }

                    break;
                }
            }
        }

        BoneList.clear();
    }

    // AimBot - ToDo
    if (target.m_address != NULL && AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode))
    {
        int boneId = 1;
        switch (g.AimTargetBone)
        {
        case 0: boneId = BONE_HEAD; break;
        case 1: boneId = BONE_NECK; break;
        case 2: boneId = BONE_SPINE; break;
        case 3: boneId = BONE_HIP; break;
        default:
            break;
        }

        Vector2 Angle = CalcAngle(pLocal->GetCameraPosition(), target.GetBoneByID(boneId));
        Vector2 ViewAngle = pLocal->GetViewAngle();
        Vector2 Delta = Angle - ViewAngle;
        NormalizeAngles(Delta);
        Vector2 SmoothedAngle = ViewAngle + (Delta / g.AimSmooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec2_Empty(SmoothedAngle))
            m.Write<Vector2>(m.m_dwClientBaseAddr + offset.dwViewAngles, SmoothedAngle);

        lastTarget = target;
    }
    else if (g.AimBotEnable) 
    {
        if (!AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode))
            lastTarget = CEntity();
        else if (target.m_address == NULL)
            lastTarget = CEntity();
    }
}