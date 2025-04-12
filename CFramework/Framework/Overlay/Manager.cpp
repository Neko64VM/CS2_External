#include "Overlay.h"

bool Overlay::InitOverlay(const char* targetName, InitializeMode InitMode)
{
    if (InitMode == WINDOW_TITLE || InitMode == WINDOW_CLASS) {
        g.hGameWindow = InitMode == WINDOW_TITLE ? FindWindowA(nullptr, targetName) : FindWindowA(targetName, nullptr);
    }
    else if (InitMode == PROCESS) {
        g.hGameWindow = GetTargetWindow(targetName);
    }
    else {
        MessageBoxA(nullptr, "Invalid overlay init option", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    if (!g.hGameWindow) {
        MessageBoxA(nullptr, "target process not found", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    // Get ClassName
    GetWindowTextA(g.hGameWindow, m_TargetTitle, sizeof(m_TargetTitle));

    return CreateOverlay();
}

void Overlay::OverlayManager()
{
    // Window Check
    HWND hWnd = FindWindowA(nullptr, m_TargetTitle);
    if (!hWnd) {
        g_ApplicationActive = false;
        return;
    }

    // オーバーレイをゲームの上に配置
    HWND ProcessHwnd = GetWindow(hWnd, GW_HWNDPREV);
    SetWindowPos(m_hWnd, ProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // ShowMenu Toggle
    if (GetKeyState(g.dwMenuKey) && !g.bShowMenu) {
        g.bShowMenu = true;
    }
    else if (!GetKeyState(g.dwMenuKey) && g.bShowMenu) {
        g.bShowMenu = false;
    }

    // サイズを取得
    RECT TmpRect{};
    POINT TmpPoint{};
    GetClientRect(hWnd, &TmpRect);
    ClientToScreen(hWnd, &TmpPoint);

    // ImGuiにマウス入力を渡す
    POINT MousePos{};
    GetCursorPos(&MousePos);
    ImGui::GetIO().MousePos = ImVec2(MousePos.x - TmpPoint.x, MousePos.y - TmpPoint.y);
    ImGui::GetIO().MouseDown[0] = utils::IsKeyDown(VK_LBUTTON);

    // Window Resizer
    if (TmpRect.left != g.rcSize.left || TmpRect.bottom != g.rcSize.bottom || TmpRect.top != g.rcSize.top || TmpRect.right != g.rcSize.right || TmpPoint.x != g.ptPoint.x || TmpPoint.y != g.ptPoint.y)
    {
        g.rcSize = TmpRect;
        g.ptPoint = TmpPoint;
        SetWindowPos(m_hWnd, nullptr, TmpPoint.x, TmpPoint.y, g.rcSize.right, g.rcSize.bottom, SWP_NOREDRAW);
    }
}

HWND Overlay::GetTargetWindow(const std::string processName)
{
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
            break;
    }

    CloseHandle(snapShot);

    HWND hwnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hwnd, 8) != 0 || !IsWindowVisible(hwnd))
            continue;
        DWORD ProcessID;
        GetWindowThreadProcessId(hwnd, &ProcessID);
        if (entry.th32ProcessID == ProcessID)
            return hwnd;
    } while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}
