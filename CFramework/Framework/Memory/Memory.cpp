#include "Memory.h"

bool Memory::AttachProcess(const char* targetName, InitializeMode InitMode)
{
    // 任意の方法でPIDを取得する
    if (InitMode == WINDOW_TITLE || InitMode == WINDOW_CLASS)
    {
        // ウィンドウベースでPIDを取得
        HWND hWindow = InitMode == WINDOW_TITLE ? FindWindowA(nullptr, targetName) : FindWindowA(targetName, nullptr);

        if (!hWindow) {
            MessageBoxA(nullptr, "please open [# TARGET GAME NAME #]", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        GetWindowThreadProcessId(hWindow, &m_dwPID);
    }
    else if (InitMode == PROCESS)
    {
        // 実行ファイル名ベースでPIDを取得
        PROCESSENTRY32 process = GetProcess(targetName);

        if (process.th32ProcessID == 0) {
            MessageBoxA(nullptr, "please open [# TARGET GAME NAME #]", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return false;
        }

        m_dwPID = process.th32ProcessID;
    }
    else
    {
        // 無効なオプション
        MessageBoxA(nullptr, "Invalid memory init option", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    // プロセスハンドルを取得
    m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

    if (!m_hProcess) {
        MessageBoxA(nullptr, "Failed to get process handle", "Init Error", MB_TOPMOST | MB_ICONERROR | MB_OK); // 管理者として実行するかアンチチートをバイパスする
        return false;
    }

    this->SetBaseAddress();

    return true;
}

void Memory::DetachProcess()
{
    CloseHandle(m_hProcess);
}

uintptr_t Memory::FindPattern(const std::vector<uint8_t>& read_data, const uintptr_t baseAddress, const std::string pattern, int offset, int extra)
{
    std::vector<uint8_t> bytes;
    std::istringstream patternStream(pattern);
    std::string byteStr;

    while (patternStream >> byteStr) {
        if (byteStr == "?" || byteStr == "??")
            bytes.push_back(0);
        else
            bytes.push_back(static_cast<uint8_t>(strtol(byteStr.c_str(), nullptr, 16)));
    }

    for (size_t i = 1000000; i < read_data.size(); ++i)
    {
        bool patternMatch = true;
        for (size_t j = 0; j < bytes.size(); ++j)
        {
            if (bytes[j] != 0 && read_data[i + j] != bytes[j])
            {
                patternMatch = false;
                break;
            }
        }

        if (patternMatch) {
            uintptr_t patternAddress = baseAddress + i;
            int32_t of;
            ReadProcessMemory(m_hProcess, reinterpret_cast<LPCVOID>(patternAddress + offset), &of, sizeof(of), nullptr);
            uintptr_t result = patternAddress + of + extra;
            bytes.clear();

            return (result - baseAddress);
        }
    }
}

MODULEINFO Memory::GetModuleInfo(const std::string moduleName)
{
    DWORD cb;
    HMODULE hMods[256]{};
    MODULEINFO modInfo{};

    if (EnumProcessModules(m_hProcess, hMods, sizeof(hMods), &cb)) 
    {
        for (unsigned int i = 0; i < (cb / sizeof(HMODULE)); i++) 
        {
            char szModName[MAX_PATH];
            if (GetModuleBaseNameA(m_hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(char)))
            {
                if (moduleName == szModName) {
                    GetModuleInformation(m_hProcess, hMods[i], &modInfo, sizeof(modInfo));
                    break;
                }
            }
        }
    }

    return modInfo;
}

uintptr_t Memory::GetModuleBaseAddress(const std::string moduleName)
{
    MODULEENTRY32 entry{};
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwPID);

    while (Module32Next(snapShot, &entry))
    {
        if (!moduleName.compare(entry.szModule))
        {
            CloseHandle(snapShot);
            return reinterpret_cast<uintptr_t>(entry.modBaseAddr);
        }
    }

    if (snapShot)
        CloseHandle(snapShot);

    return reinterpret_cast<uintptr_t>(entry.modBaseAddr);
}

MODULEENTRY32 Memory::GetModule(const std::string moduleName)
{
    MODULEENTRY32 entry{};
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwPID);

    while (Module32Next(snapShot, &entry))
    {
        if (!moduleName.compare(entry.szModule))
        {
            CloseHandle(snapShot);
            return entry;
        }
    }

    if (snapShot)
        CloseHandle(snapShot);

    return entry;
}


PROCESSENTRY32 Memory::GetProcess(const std::string processName)
{
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
        {
            CloseHandle(snapShot);
            return entry;
        }
    }

    CloseHandle(snapShot);

    return PROCESSENTRY32();
}

Memory m;