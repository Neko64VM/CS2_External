#include "CEntity.h"

// 毎フレーム更新する必要のある情報
bool CEntity::Update()
{
	m_vOldOrigin	  = m.Read<Vector3>(m_pCSPlayerPawn + Offset::m_vOldOrigin);
	m_iHealth		  = m.Read<int>(m_pCSPlayerPawn + Offset::m_iHealth);
	
	if (Vec3_Empty(m_vOldOrigin) || m_iHealth <= 0)
		return false;

	m_iIDEntIndex = m.Read<uint32_t>(m_pCSPlayerPawn + Offset::m_iIDEntIndex);

	return true;
}

bool CEntity::TriggerAllow(const uintptr_t& entitylist, CEntity* local)
{
	// pointer
	uintptr_t list = m.Read<uintptr_t>(entitylist + 0x8 * ((local->m_iIDEntIndex & 0x7FFF) >> 9) + 0x10);
	m_pCSPlayerPawn = m.Read<uintptr_t>(list + 120 * (local->m_iIDEntIndex & 0x1FF));

	if (!Update())
		return false;

	m_iTeamNum = m.Read<int>(m_address + Offset::m_iTeamNum);

	return true;
}

// 毎フレーム更新する必要のない情報
bool CEntity::UpdateStatic(const uintptr_t& entitylist)
{
	//std::unique_lock<std::shared_mutex> lock(mtx);

	// pointer
	uint32_t pawn = m.Read<uint32_t>(m_address + Offset::m_hPlayerPawn);
	uintptr_t list = m.Read<uintptr_t>(entitylist + 0x8 * ((pawn & 0x7FFF) >> 9) + 0x10);

	if (list == NULL)
		return false;

	// pointers
	m_pCSPlayerPawn = m.Read<uintptr_t>(list + 120 * (pawn & 0x1FF));
	m_pCollision = m.Read<uintptr_t>(m_pCSPlayerPawn + Offset::m_pCollision);
	m_pGameSceneNode = m.Read<uintptr_t>(m_pCSPlayerPawn + Offset::m_pGameSceneNode);
	m_pBoneArray = m.Read<uintptr_t>(m_pGameSceneNode + (Offset::m_modelState + 0x80));
	m_pCameraServices = m.Read<uintptr_t>(m_pCSPlayerPawn + Offset::m_pCameraServices);
	m_pClippingWeapon = m.Read<uintptr_t>(m_pCSPlayerPawn + Offset::m_pClippingWeapon);

	// entity data
	m_iTeamNum = m.Read<int>(m_address + Offset::m_iTeamNum);
	m_iMaxHealth = m.Read<int>(m_pCSPlayerPawn + Offset::m_iMaxHealth);
	
	// Name
	uintptr_t nameAddress = m.Read<uintptr_t>(m_address + Offset::m_sSanitizedPlayerName);

	if (nameAddress != NULL) {
		m.ReadString(nameAddress, &m_namePlayer, sizeof(m_namePlayer));
	}

	// Weapon name
	uintptr_t pWeaponEntity = m.ReadChain(m_pClippingWeapon, { 0x10, 0x20 });

	if (pWeaponEntity != NULL) {
		m_nameWeapon = m.ReadStringA(pWeaponEntity);

		if (m_nameWeapon.find("weapon_") != std::string::npos)
			m_nameWeapon = m_nameWeapon.substr(7, m_nameWeapon.length());
	}

	return true;
}

Vector3 CEntity::vecMin()
{
	return m.Read<Vector3>(m_pCollision + 0x40) + m_vOldOrigin;
}

Vector3 CEntity::vecMax()
{
	return m.Read<Vector3>(m_pCollision +  0x4C) + m_vOldOrigin;
}

BoundingBox CEntity::GetBoundingBoxData(Matrix& ViewMatrix)
{
	BoundingBox bbOut{};

	Vector3 min = vecMin();
	Vector3 max = vecMax();

	Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

	Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
				Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
				Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

	if (!WorldToScreen(ViewMatrix, g.rcSize, points[3], flb) || !WorldToScreen(ViewMatrix, g.rcSize, points[5], brt) ||
		!WorldToScreen(ViewMatrix, g.rcSize, points[0], blb) || !WorldToScreen(ViewMatrix, g.rcSize, points[4], frt) ||
		!WorldToScreen(ViewMatrix, g.rcSize, points[2], frb) || !WorldToScreen(ViewMatrix, g.rcSize, points[1], brb) ||
		!WorldToScreen(ViewMatrix, g.rcSize, points[6], blt) || !WorldToScreen(ViewMatrix, g.rcSize, points[7], flt))
		return bbOut;

	Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
	bbOut.left = flb.x;
	bbOut.top = flb.y;
	bbOut.right = flb.x;
	bbOut.bottom = flb.y;

	for (auto j = 1; j < 8; ++j)
	{
		if (bbOut.left > vec2_array[j].x)
			bbOut.left = vec2_array[j].x;
		if (bbOut.bottom < vec2_array[j].y)
			bbOut.bottom = vec2_array[j].y;
		if (bbOut.right < vec2_array[j].x)
			bbOut.right = vec2_array[j].x;
		if (bbOut.top > vec2_array[j].y)
			bbOut.top = vec2_array[j].y;
	}

	return bbOut;
}

uint32_t CEntity::GetFlag()
{
	return m.Read<int>(m_address + Offset::m_fFlags);
}

bool CEntity::IsDead()
{
	return m.Read<int>(m_address + Offset::m_lifeState) > 0;
}

Vector3 CEntity::GetBoneByID(const int ID)
{
	return m.Read<Vector3>(m_pBoneArray + (ID * 0x20));
}

std::vector<Vector3> CEntity::GetBoneList()
{
	AllBones ab = m.Read<AllBones>(m_pBoneArray), *all_bone = &ab;
	std::vector<Vector3> list(32);

	for (int i = 0; i < 32; i++)
	{
		if (Vec3_Empty(all_bone->bone[i].position)) {
			list.clear();
			return list;
		}
		
		list[i] = all_bone->bone[i].position;
	}

	return list;
}

Vector3 CEntity::GetCameraPosition()
{
	return m.Read<Vector3>(m_pCSPlayerPawn + Offset::m_vecLastClipCameraPos);
}

Vector2 CEntity::GetViewAngle()
{
	return m.Read<Vector2>(m.m_dwClientBaseAddr + Offset::dwViewAngles);
}