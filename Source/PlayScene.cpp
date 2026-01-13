#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Engine/SceneManager.h"
#include "Stage.h"
#include "Engine/Input.h"

PlayScene::PlayScene(GameObject* parent)
	:GameObject(parent, "PlayScene")
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	//Player* pPlayer = new Player(this);
	//pPlayer->Initialize();
	//childList_.push_back(pPlayer);

	/*Instantiate<Player>(this);
	Instantiate<Enemy>(this);*/
	Instantiate<Stage>(this);
}

void PlayScene::Update()
{
	if (Input::IsKeyDown(DIK_T))
	{
		SceneManager* sceneOb = (SceneManager*)FindObject("SceneManager");
		sceneOb->ChangeScene(SCENE_ID_TEST);
	}
}

void PlayScene::Draw()
{
}

void PlayScene::Release()
{
}
