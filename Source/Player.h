#pragma once
#include "Engine/GameObject.h"

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

private:
	ChildOden* pRChildOden;
	ChildOden* pLChildOden;
	Bullet* bullet_;
	int hModel_;
};

