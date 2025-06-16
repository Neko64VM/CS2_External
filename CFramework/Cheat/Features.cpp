#include "CFramework.h"

bool CFramework::AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode)
{
    // 無条件でAimBotを有効にする
    if (AimKeyMode == 0)
        return true;

    // 前提チェック(共通)
    if (AimKey0 == NULL || !utils::IsKeyDown(AimKey0) && !utils::IsKeyDown(AimKey1) || g.bShowMenu) {
        return false;
    }

    switch (AimKeyMode)
    {
    case 1: // and
        if (AimKey1 == NULL && utils::IsKeyDown(AimKey0))
            break;
        else if (!utils::IsKeyDown(AimKey0) || !utils::IsKeyDown(AimKey1))
            return false;
        else if (!utils::IsKeyDown(AimKey0))
            return false;
        break;
    case 2: // or
        if (AimKey1 == NULL && utils::IsKeyDown(AimKey0))
            break;
        else if (utils::IsKeyDown(AimKey0) || utils::IsKeyDown(AimKey1))
            break;

        break;
    }

    return true;
}