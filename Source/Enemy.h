#pragma once
#include "Engine/GameObject.h"

class Fbx;

class Enemy : public GameObject
{
public:
	Enemy(GameObject* parent);
	~Enemy();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void OnCollision(GameObject* pTarget) override;

	bool isDed() { return eDed_; }
private:
	Fbx* pFbx_;
	int hModel_;
	bool eDed_;
};