#include "Stage.h"
#include "Player.h"
#include "Enemy.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../resource.h"
#include "StageObject.h"

namespace
{
	const int ENEMY_NUM = 2;
}

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), hModel_(-1)
{
	/*for (int j = 0;j < ZSIZE;j++) {
		for (int i = 0;i < XSIZE;i++)
		{
			SetBlock(BLOCK_TYPE::WATER, i, j);
			SetBlockHeight(i, j, 1 + rand() % 14);
		}
	}*/
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	/*hModel_ = Model::Load("texture.fbx");
	assert(hModel_ >= 0);
	transform_.position_= { 0.0f,-8.0f,5.0f};

	transform_.scale_ = { 1.0f,1.0f,1.0f };*/

	Instantiate<Player>(this);
	/*enemy_ = std::vector<Enemy*>(ENEMY_NUM);
	for (int i = 0;i < ENEMY_NUM;i++)
	{
		int x = 0;
		x = rand() % 11;
		if (i % 2 == 0)
		{
			x = -x;
		}
		
		enemy_[i] = (Enemy*)Instantiate<Enemy>(this);
		enemy_[i]->SetPosition((float)x, 0.0f, 50.0f);
	}*/


	std::vector<string> modelName
	{
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};
	for (int i = 0;i < modelName.size();i++)
	{
		hModel_[i] = Model::Load(modelName[i]);
		assert(hModel_[i] >= 0);
	}
	hModelColl_ = Model::Load("BoxDefault.fbx");

	/*for (int j = 0;j < ZSIZE;j++) {
		for (int i = 0;i < XSIZE;i++) {
			Transform t;
			t.position_ = { i,0,j };
			if (j % 2 && i % 3)
			{
				t.position_.y = 1;
			}
			t.rotate_ = { 0,0,0 };
			t.scale_ = { 1.0f,1.0f,1.0f };
			t.Calculation();
			Model::SetTransform(hModel_[BLOCK_TYPE::DEFAULT], t);
			models_.push_back(hModel_[BLOCK_TYPE::DEFAULT]);
		}
	}*/

	
	
	for (int j = 0; j < ZSIZE; j++) {
		for (int i = 0; i < XSIZE; i++) {

			int h = Model::Load("BoxDefault.fbx");
			assert(h >= 0);

			Transform t;
			t.position_ = { (float)i, 0.0f, (float)j };
			if (j % 2 == 0 && i % 3 == 0) {
				t.position_.y = 1.0f;
			}
			t.scale_ = { 1,1,1 };
			t.rotate_ = { 0,0,0 };
			t.Calculation();

			Model::SetTransform(h, t);
			models_.push_back({h,t});
		}
	}
}

void Stage::Update()
{
	//RayCastData data;
	//Player* p = (Player*)FindObject("Player");
	//XMFLOAT3 pPos = p->GetPos();
	//XMStoreFloat4(&data.start,XMLoadFloat3(&pPos));
	//XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//XMStoreFloat4(&data.dir, dir);


	//Transform trans;
	//trans.position_.x = 0.0f;
	//trans.position_.y = -8.0f;
	//trans.position_.z = 5.0f;
	//Model::SetTransform(hModel_, trans);
	//Model::RayCast(hModel_, data);
	//
	////⑥ レイが当たったらブレークポイントで止める
	//if (data.isHit)
	//{
	//	//なんか書いてないとifに当たらないから、
	//	int a = 0;
	//	a++;
	//}

	//for (auto it = enemy_.begin(); it != enemy_.end(); )
	//{
	//	Enemy* e = *it;
	//	if (e->isDed())
	//	{
	//		it = enemy_.erase(it); // ベクターから削除
	//	}
	//	else
	//	{
	//		++it;
	//	}
	//}

	//if (enemy_.empty())
	//{
	//	KillMe();
	//	SceneManager* sceneOb = (SceneManager*)FindObject("SceneManager");
	//	sceneOb->ChangeScene(SCENE_ID_CLEAR);
	//}
	// 
	
	////視点移動をする
	//if (Input::IsKey(DIK_RIGHT))
	//{
	//	transform_.rotate_.y += 0.1f;
	//}
	//if (Input::IsKey(DIK_LEFT))
	//{
	//	transform_.rotate_.y -= 0.1f;
	//}

	//XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	//XMMATRIX mRot = XMMatrixRotationRollPitchYaw(transform_.rotate_.x, transform_.rotate_.y, 0);
	//XMVECTOR vMove = { 0,0,0.1f };
	//vMove = XMVector3TransformCoord(vMove, mRot);

	//if (Input::IsKey(DIK_W))
	//{
	//	vPos += vMove;
	//	XMStoreFloat3(&transform_.position_, vPos);
	//}

	////XMVECTOR camPos = XMLoadFloat3(&transform_.position_);
	////XMFLOAT3 vTarget = transform_.position_;
	////vTarget = vTarget + camPos;

	////Camera::SetPosition(camPos);
	////Camera::SetTarget(vTarget);

	//XMVECTOR vCam = { 0,5.0f,-10.0f,0 };
	//vCam = XMVector3TransformCoord(vCam, mRot);
	//XMFLOAT3 camPos;
	//XMStoreFloat3(&camPos, vPos + vCam);
	//Camera::SetPosition(camPos);

	//XMFLOAT3 camTarget = transform_.position_;
	//Camera::SetTarget(camTarget);


	//XMVECTOR vDir = XMVectorSubtract(XMLoadFloat3(&camPos), XMLoadFloat3(&camTarget));
	//vDir = XMVector3Normalize(vDir);
	//XMStoreFloat3(&camTarget, vDir);

	//RayCastData data{
	//	{ camPos.x,camPos.y,camPos.z,1 },
	//	{camTarget.x,camPos.y,camPos.z,0},
	//	false,
	//	100
	//};
	//Model::RayCast(hModelColl_, data);
	//if (data.isHit)
	//{
	//	int i = 0;
	//	i++;
	//}

}

void Stage::Draw()
{
	/*Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);*/

	//Player* pl = (Player*)FindObject("Player"); //プレイヤーのポジションの真下にブロックを置きたいので持ってくる
	//if (!pl) return; //念のためプレイヤーが存在しているか確認
	//static XMFLOAT3 bPos = pl->GetPos();//ポジションを持ってくる
	//bPos.y = -3.0f;
	//Transform bt;
	//bt.position_.x = bPos.x;
	//bt.position_.y = bPos.y;
	//bt.position_.z = bPos.z;
	//Model::SetTransform(hModelColl_, bt);
	//Model::Draw(hModelColl_);


	/////////////
	
	//for (int j = 0;j < ZSIZE;j++) {
	//	for (int i = 0;i < XSIZE;i++)
	//	{
	//		type_ = BLOCK_TYPE::DEFAULT;
	//		Transform t;
	//		t.position_.x = i;
	//		t.position_.z = j;
	//		if (i % 2 == 0 && j % 3 == 0)
	//			t.position_.y = 1.0f;
	//		else
	//			t.position_.y = 0.0f;
	//		t.scale_ = { 1.0,1.0,1.0 };
	//		Model::SetTransform(hModel_[type_], t);
	//		Model::Draw(hModel_[type_]);

	//		// 適当にブロックを積み上げてるコード
	//		/*for (int k = 0; k < (int)(GetT(i, j).height); k++) {
	//			int type = (int)(GetT(i, j).type);
	//			Transform t;
	//			t.position_.x = i - 15 / 2.0;
	//			t.position_.z = j;
	//			t.position_.y = k - 15;
	//			t.scale_ = { 0.95, 0.95, 0.95 };
	//			Model::SetTransform(hModel_[type], t);
	//			Model::Draw(hModel_[type]);
	//		}*/ 
	//	}
	//}

	/////////////

	for (auto& b : models_) {
		Model::SetTransform(b.type, b.transform);
		Model::Draw(b.type);
	}



	Transform t;
	t.position_.x = 5;
	t.position_.z = 5;
	t.position_.y = 5;
	t.scale_ = { 0.95, 0.95, 0.95 };
	int type = BLOCK_TYPE::WATER;
	Model::SetTransform(hModel_[type], t);
	Model::Draw(hModel_[type]);

	RayCastData rayData{
		{ 5.0f, 0.0f, 5.0f,1.0f },
		{ 0.0f,-1.0f, 0.0f,0.0f },
		false,
		100.0f
	};
	Model::RayCast(hModel_[BLOCK_TYPE::DEFAULT], rayData);
	if (rayData.isHit)
	{
		MessageBoxA(NULL, "Hit", "Info", MB_OK);
	}
}

void Stage::Release()
{
	KillMe();
}

void Stage::OnCollision(GameObject* pTarget)
{
	
}

bool Stage::hitObject(RayCastData& data)
{
	int mol = 0;
	int hitModel;
	if (Model::RayCastAll(mol, data, hitModel)) {
		return true;
	}
	else
		return false;


	//bool hit = false;
	//data.dist = data.maxDist+10.0f;

	//for (int i = 0; i < MODEL_NUM; i++) {//モデルのIDごとに判定
	//	Model::RayCast(hModel_[i], data);
	//	if (data.isHit) {
	//		hit = true;
	//	}
	//}

	/*for (int j = 0; j < ZSIZE; j++) {
		for (int i = 0; i < XSIZE; i++)	{
			// 今描画してるブロックと同じ transform を作る
			Transform t;
			t.position_.x = i;
			t.position_.z = j;

			if (i % 2 == 0 && j % 3 == 0)
				t.position_.y = -5.0f;
			else
				t.position_.y = -4.0f;

			t.scale_ = { 1.0f, 1.0f, 1.0f };

			int type = BLOCK_TYPE::DEFAULT;

			Model::SetTransform(hModel_[type], t);

			// レイキャスト
			Model::RayCast(hModel_[type], data);

			if (data.isHit)
			{
				hit = true;
			}
		}
	}*/

	
	/*Transform t;
	t.position_.x = data.start.x;
	t.position_.y = data.start.y;
	t.position_.z = data.start.z;
	int type = BLOCK_TYPE::WATER;
	Model::SetTransform(hModel_[type], t);

	Model::RayCast(hModel_[BLOCK_TYPE::DEFAULT], data);
	if (data.isHit) {
		hit = true;
	}
	return hit;*/

	/*for (auto& h : models_) {
		Model::RayCast(h.type, data);
	}
	if (data.isHit) {
		hit = true;
	}
	return hit;*/
}

//bool Stage::CollideLine(RayCastData& data)
//{
//	bool found = false;
//	std::vector<StageObject*> objs = (StageObject*)FindObject("StageObject");
//	auto& objs = stage->GetStageObjects();
//	for (StageObject* obj : objs)
//	{
//		obj->RayCast(data);
//	}
//}

// スフィアコライダーやら関係
