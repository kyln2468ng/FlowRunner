#include "Goal.h"
#include "../Engine/Model.h"
#include "../Engine/SphereCollider.h"

Goal::Goal(GameObject* parent)
	:StageObject(parent)
{
	isGoal_ = false;
	hModel_ = -1;
}

Goal::~Goal()
{
}

void Goal::Initialize()
{
	gModel_ = Model::Load("BoxDefault.fbx");
	transform_.position_ = { 0.0f,5.0f,8.0f };
	transform_.scale_ = { 2.0f,2.0f,2.0f };
	transform_.Calculation();

	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);
}

void Goal::Draw()
{
	Model::SetTransform(gModel_, transform_);
	Model::Draw(gModel_);
	OutputDebugStringA("goal");
}

void Goal::Update()
{
}

void Goal::Release()
{
}

bool Goal::IsGoal()
{
	return isGoal_;
}

void Goal::OnCollision(GameObject* pTarget)
{
	isGoal_ = true;
}
