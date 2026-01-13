#include "Bullet.h"
#include "../Engine/Model.h"
#include "../Engine/SphereCollider.h"

Bullet::Bullet(GameObject* parent)
	:GameObject(parent,"Bullet"),hModel_(-1),life_(0)
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize()
{
	hModel_ = Model::Load("Bullet.fbx");
	assert(hModel_ >= 0);
	transform_.scale_ = { 0.5f,0.5f,0.5f };
	
	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);
	life_ = 60 * 5;
}

void Bullet::Update()
{
	transform_.position_.z += 0.5f;
	life_--;
	if (life_ < 0)
	{
		this->Release();
	}
}

void Bullet::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Bullet::Release()
{
	KillMe();
}

