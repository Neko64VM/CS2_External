#include "Offset.h"
#include "../../Framework/Memory/Memory.h"

bool GOffset::InitOffset()
{
	// PatternScan.
    auto info = m.GetModuleInfo("client.dll");
    std::vector<uint8_t> bytes = m.ReadBytes((uintptr_t)info.lpBaseOfDll, info.SizeOfImage);

    dwEntityList = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 89 35 ?? ?? ?? ?? 48 85 f6", 3, 7);
    printf("dwEntityList : 0x%I64x\n", dwEntityList);

    dwViewMatrix = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 8d 0d ?? ?? ?? ?? 48 c1 e0 06", 3, 7);
    printf("dwViewMatrix : 0x%I64x\n", dwViewMatrix);

    dwLocalPlayerController = m.FindPattern(bytes, m.m_dwClientBaseAddr, "48 89 05 ?? ?? ?? ?? 8b 9e", 3, 7);
    printf("dwLocalPlayerController : 0x%I64x\n", dwLocalPlayerController);

    dwViewAngles = cs2_dumper::offsets::client_dll::dwViewAngles;
    printf("dwViewAngles : 0x%I64x\n", dwViewAngles);

    bytes.clear();

    return dwEntityList > 0x1A00000 && dwLocalPlayerController > 0x1A00000 && dwViewMatrix > 0x1A00000;
}

uintptr_t GOffset::GetLocal()
{
    return m.Read<uintptr_t>(m.m_dwClientBaseAddr + dwLocalPlayerController);
}

GOffset offset;