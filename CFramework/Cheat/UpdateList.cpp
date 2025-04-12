#include "CFramework.h"

const int ReadCount = 1024;

std::vector<CEntity> CFramework::GetEntityList() {
    std::lock_guard<std::mutex> lock(list_mutex);
    return EntityList;
}

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(333));

        CEntity local = CEntity();

        // Read EntityList
        auto list_addr = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset.dwEntityList);

        if (list_addr == NULL)
            continue;

        // Gte Local
        local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset.dwLocalPlayerController);

        if (!local.UpdateStatic(list_addr))
            continue;
        else if (!local.Update())
            continue;

        std::vector<CEntity> list_result;

        for (int i = 0; i < ReadCount; i++)
        {
            uintptr_t entity_entry = m.Read<uintptr_t>(list_addr + (0x8 * (i & 0x7FFF) >> 9) + 0x10);

            if (entity_entry == NULL)
                continue;
            else if (entity_entry == local.m_address)
                continue;

            CEntity p = CEntity();
            p.m_address = m.Read<uintptr_t>(entity_entry + 120 * (i & 0x1FF));

            uintptr_t classNamePtr = m.ReadChain(p.m_address, { 0x10, 0x20 });
            std::string class_name = m.ReadStringA(classNamePtr);

            if (class_name.size() > 0)
            {
                // if entity is player
                if (!class_name.compare("cs_player_controller"))
                {
                    if (!p.UpdateStatic(list_addr))
                        continue;
                    else if (!p.Update())
                        continue;

                    p.m_nameClass = class_name;
                    list_result.push_back(p);
                    continue;
                }
            }
        }

        std::lock_guard<std::mutex> lock(list_mutex);
        EntityList = list_result;
    }
}