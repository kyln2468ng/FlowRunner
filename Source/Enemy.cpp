#include "Enemy.h"
#include "Engine/Model.h"
#include "Engine/Fbx.h"
#include "Engine/SphereCollider.h"
#include "Engine/SceneManager.h"

Enemy::Enemy(GameObject* parent)
	:GameObject(parent,"Enemy"),pFbx_(nullptr),hModel_(-1),eDed_(false)
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
	hModel_ = Model::Load("rCubes.fbx");
	assert(hModel_ >= 0);
	transform_.scale_.x = 0.7f;
	transform_.scale_.y = 0.7f;
	transform_.scale_.z = 0.7f;
	transform_.position_ = { 0.0f, 0.0f, 50.0f };

	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);
}

void Enemy::Update()
{
	//transform_.position_.z -= 0.2f;
}

void Enemy::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Enemy::Release()
{
	if(pFbx_)
	{
		pFbx_->Release();
		delete pFbx_;
		pFbx_ = nullptr;
	}
}

void Enemy::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() == "Bullet")
	{
		eDed_ = true;
		KillMe();
	}
	
}
