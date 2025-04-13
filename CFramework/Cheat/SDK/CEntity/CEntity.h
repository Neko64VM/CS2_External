#pragma once
#include "../GameSDK/GameSDK.h"

struct Bone {
	Vector3 position;
	uint64_t address[2];
};

struct AllBones {
	Bone bone[32]{};
};

struct BoundingBox {
	int left, right, top, bottom;
};

class CEntity
{
private:
	
public:
    uintptr_t m_address;
	uintptr_t m_pCSPlayerPawn;
	uintptr_t m_pGameSceneNode;
	uintptr_t m_pCameraServices;
	uintptr_t m_pCollision;
	uintptr_t m_pClippingWeapon;
	uintptr_t m_pBoneArray;

	// Player
	Vector3 m_vecAbsVelocity;
	Vector3 m_vOldOrigin;

	int m_iTeamNum;
	int m_lifeState;
	int m_iHealth;
	int m_iMaxHealth;
	uint32_t m_iIDEntIndex;
	char m_namePlayer[32];
	std::string m_nameWeapon;
	std::string m_nameClass;

    // Functions
	bool Update();
	bool UpdateStatic(const uintptr_t& entitylist);
	bool IsDead();

	Vector3 vecMin();
	Vector3 vecMax();
	BoundingBox GetBoundingBoxData(Matrix& ViewMatrix);

	uint32_t GetFlag();
	Vector3 GetBoneByID(const int ID);
	std::vector<Vector3> GetBoneList();

	Vector2 GetViewAngle();
	Vector3 GetCameraPosition();
};