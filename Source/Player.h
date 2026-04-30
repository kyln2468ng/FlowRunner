#pragma once
#include "../Engine/GameObject.h"
#include "PlayerParamConfig.h"

class ChildOden;
class Bullet;

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
	void WallCling(DirectX::XMVECTOR hitPos_,DirectX::XMVECTOR normal_); //壁の吸着
	void JudgeWall(XMVECTOR& vPos, XMVECTOR& move, const XMVECTOR& normal, float dist);//壁判定。めり込み防止と押し戻し

	ChildOden* pRChildOden;
	ChildOden* pLChildOden;
	Bullet* bullet_;
	int hModel_;
	bool onGround_;
	float velocityY;
	PlayerParamConfig param_;

	bool isWall_;
	XMFLOAT3 wallNormal_;
};

