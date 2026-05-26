#pragma once
#include "../Engine/GameObject.h"
#include "PlayerParamConfig.h"
#include "../Engine/Animation.h"
#include "../Engine/Fbx.h"

class ChildOden;
class Bullet;

struct WallHitData
{
	bool isHit = false;
	float dist = FLT_MAX;
	XMVECTOR normal = XMVectorZero();
	XMVECTOR hitPos = XMVectorZero();
};


class Player : public GameObject
{
public:
	Player(GameObject* parent);
	~Player();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void OnCollision(GameObject* pTarget) override;

	XMFLOAT3 GetPos() { return transform_.position_; }
	int GetHandle() { return hModel_; }

private:
	WallHitData DetectWall(const XMVECTOR& vPos, const XMVECTOR& move, const XMVECTOR& right);
	
	void WallCling(const WallHitData& wall); //壁の吸着
	void WallCollision(XMVECTOR& vPos, XMVECTOR& move, const WallHitData& wall);//壁判定。めり込み防止と押し戻し
	void WallMove(XMVECTOR& move, const WallHitData& wall);
	void WallJump(const WallHitData& wall);

	ChildOden* pRChildOden;
	ChildOden* pLChildOden;
	Bullet* bullet_;
	int hModel_;
	bool onGround_;
	//float velocityY;
	PlayerParamConfig param_;
	XMFLOAT3 velocity_;

	bool isWall_;
	XMFLOAT3 wallNormal_;
	Animation anim_;
	Fbx* model_;
};

