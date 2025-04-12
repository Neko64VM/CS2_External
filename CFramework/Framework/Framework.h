#pragma once
#include "Config/Config.h"
#include "Memory/Memory.h"
#include "SimpleMath/SimpleMath.h"
#include <vector>
#include <thread>
#include <string>
#include <shared_mutex>
using namespace DirectX::SimpleMath;

#ifndef UTILS_H
#define UTILS_H

namespace utils
{
    static HHOOK hKeyboardHook;
    LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    void EnableKeyboardHook();
    void DisableKeyboardHook();

    const char* KeyNames[];
    bool IsKeyDown(DWORD VK);
    void KeyBinder(DWORD& target_key, int& BindID);

    std::vector<std::string> GetPhysicalDriveList();
    std::string ConvertWideToMultiByte(const std::wstring& target_str);

    namespace file
    {
        bool IsExistsFile(const std::string path);
        bool IsExistsDirectory(const std::string path);
        void SelectFilePath(std::string& vOut);
        void SelectDirectoryPath(const char* str, std::string& vOut);
        bool DoesFileExistInDirectory(const std::string path, const std::string file);
        std::string GetAppDataPath(const GUID id);
        std::vector<std::string> GetFileList(const std::string path, const std::string extension);
        std::optional<std::string> FindDirectory(const std::string& startDir, const std::string& targetName);
    }
    namespace process
    {
        DWORD GetProcessIDByName(const std::string processName);
        bool IsProcessRunning(const std::string processName);
        bool StartProcess(const std::string targetPath, const std::string options);
        bool StopProcess(const std::string processName);
    }
}

#endif