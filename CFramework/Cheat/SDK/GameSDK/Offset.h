#pragma once
#include "a2x/buttons.hpp"
#include "a2x/client_dll.hpp"
#include "a2x/offsets.hpp"
using namespace cs2_dumper::schemas::client_dll;

namespace Offset
{
	// Address
	constexpr auto dwViewAngles = cs2_dumper::offsets::client_dll::dwViewAngles;

	// Button
	constexpr auto dwForceJump = cs2_dumper::buttons::jump;

	// Offsets
	constexpr auto m_hPlayerPawn = CCSPlayerController::m_hPlayerPawn;
	constexpr auto m_sSanitizedPlayerName = CCSPlayerController::m_sSanitizedPlayerName;
	constexpr auto m_fFlags = C_BaseEntity::m_fFlags;
	constexpr auto m_iHealth = C_BaseEntity::m_iHealth;
	constexpr auto m_iMaxHealth = C_BaseEntity::m_iMaxHealth;
	constexpr auto m_iTeamNum = C_BaseEntity::m_iTeamNum;
	constexpr auto m_lifeState = C_BaseEntity::m_lifeState;
	constexpr auto m_pCollision = C_BaseEntity::m_pCollision;
	constexpr auto m_pGameSceneNode = C_BaseEntity::m_pGameSceneNode;
	constexpr auto m_vecLastClipCameraPos = C_CSPlayerPawnBase::m_vecLastClipCameraPos;
	constexpr auto m_angEyeAngles = C_CSPlayerPawnBase::m_angEyeAngles;
	constexpr auto m_iIDEntIndex = C_CSPlayerPawnBase::m_iIDEntIndex;
	constexpr auto m_pClippingWeapon = C_CSPlayerPawnBase::m_pClippingWeapon;
	constexpr auto m_vOldOrigin = C_BasePlayerPawn::m_vOldOrigin;
	constexpr auto m_pCameraServices = C_BasePlayerPawn::m_pCameraServices;
	constexpr auto m_modelState = CSkeletonInstance::m_modelState;
	constexpr auto m_szName = CCSWeaponBaseVData::m_szName;
};

class GOffset
{
public:
	bool InitOffset();
	uintptr_t GetLocal();

	uintptr_t dwViewAngles;
	uintptr_t dwViewMatrix;
	uintptr_t dwEntityList;
	uintptr_t dwLocalPlayerController;
};

extern GOffset offset;