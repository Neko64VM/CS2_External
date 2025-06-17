#pragma once
#include "a2x/buttons.hpp"
#include "a2x/client_dll.hpp"
#include "a2x/offsets.hpp"
using namespace cs2_dumper::schemas::client_dll;

namespace Offset
{
	// Address
	constexpr auto dwViewAngles = cs2_dumper::offsets::client_dll::dwViewAngles;

	// Offsets
	constexpr auto m_hPlayerPawn = CCSPlayerController::m_hPlayerPawn;
	constexpr auto m_sSanitizedPlayerName = CCSPlayerController::m_sSanitizedPlayerName;
	constexpr auto m_fFlags = C_BaseEntity::m_fFlags;
	constexpr auto m_iHealth = C_BaseEntity::m_iHealth;
	constexpr auto m_iMaxHealth = C_BaseEntity::m_iMaxHealth;
	constexpr auto m_iTeamNum = C_BaseEntity::m_iTeamNum;
	constexpr auto m_pCollision = C_BaseEntity::m_pCollision;
	constexpr auto m_pGameSceneNode = C_BaseEntity::m_pGameSceneNode;
	constexpr auto m_vecLastClipCameraPos = C_CSPlayerPawnBase::m_vecLastClipCameraPos;
	constexpr auto m_angEyeAngles = C_CSPlayerPawnBase::m_angEyeAngles;
	constexpr auto m_pClippingWeapon = C_CSPlayerPawnBase::m_pClippingWeapon;
	constexpr auto m_vOldOrigin = C_BasePlayerPawn::m_vOldOrigin;
	constexpr auto m_modelState = CSkeletonInstance::m_modelState;
	constexpr auto m_bPawnIsAlive = CCSPlayerController::m_bPawnIsAlive;
};

class GOffset
{
public:
	bool InitOffset();

	uintptr_t dwViewAngles{ 0 };
	uintptr_t dwViewMatrix{ 0 };
	uintptr_t dwEntityList{ 0 };
	uintptr_t dwLocalPlayerController{ 0 };
	uintptr_t dwGlobalVars{ 0 };
	uintptr_t dwPlantedC4{ 0 };
};

extern GOffset offset;