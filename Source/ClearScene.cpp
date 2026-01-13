#include "ClearScene.h"
#include "../Engine/Input.h"
#include "../Engine/Model.h"
#include "../Engine/SceneManager.h"

ClearScene::ClearScene(GameObject* parent)
	:GameObject(parent,"ClearScene"),hModel_(-1)
{
}

ClearScene::~ClearScene()
{
}

void ClearScene::Initialize()
{
	hModel_ = Model::Load("Clear.fbx");
	assert(hModel_ >= 0);
	transform_.scale_ = { 1.0f,1.0f,1.0f };
	transform_.position_ = { 0.0f,5.0f,40.0f };
}

void ClearScene::Update()
{
	float tPosY = -15.0;

	float nPos = tPosY - transform_.position_.y;
	transform_.position_.y += nPos * 0.1f;

	if (fabs(nPos) < 0.01f)
	{
		transform_.position_.y = tPosY;
	}

	if (Input::IsKeyDown(DIK_T))
	{
		SceneManager* sceneOb = (SceneManager*)FindObject("SceneManager");
		sceneOb->ChangeScene(SCENE_ID_TEST);
	}
}

void ClearScene::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void ClearScene::Release()
{
	KillMe();
}
