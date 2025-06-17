#include "Offset.h"
#include "../../Framework/Memory/Memory.h"

bool GOffset::InitOffset()
{
	// PatternScan.
    auto moduleInfo = m.GetModuleInfo("client.dll");
    std::vector<uint8_t> bytes = m.ReadBytes((uintptr_t)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);

    dwEntityList = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 89 35 ?? ?? ?? ?? 48 85 f6", 3, 7);
    printf("dwEntityList : 0x%I64x\n", dwEntityList);

    dwViewMatrix = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 8d 0d ?? ?? ?? ?? 48 c1 e0 06", 3, 7);
    printf("dwViewMatrix : 0x%I64x\n", dwViewMatrix);

    dwLocalPlayerController = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 89 05 ?? ?? ?? ?? 8b 9e", 3, 7);
    printf("dwLocalPlayerController : 0x%I64x\n", dwLocalPlayerController);

    dwGlobalVars = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 89 15 ?? ?? ?? ?? 48 89 42", 3, 7);
    printf("dwGlobalVars : 0x%I64x\n", dwGlobalVars);

    dwPlantedC4 = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 8b 15 ?? ?? ?? ?? 41 ff c0", 3, 7);
    printf("dwPlantedC4 : 0x%I64x\n", dwPlantedC4);

    dwViewAngles = cs2_dumper::offsets::client_dll::dwViewAngles;
    printf("dwViewAngles : 0x%I64x\n", dwViewAngles);

    bytes.clear();

    return dwEntityList > 0x1000000 && dwLocalPlayerController > 0x1000000 && dwViewMatrix > 0x1000000;
}

GOffset offset;