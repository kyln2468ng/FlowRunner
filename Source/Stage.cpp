#include "Stage.h"
#include "Player.h"
#include "Enemy.h"
#include "Engine/SceneManager.h"
#include "Engine/Model.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"
#include "resource.h"

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

	//Instantiate<Player>(this);
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
	
	//視点移動をする
	if (Input::IsKey(DIK_RIGHT))
	{
		transform_.rotate_.y += 0.1f;
	}
	if (Input::IsKey(DIK_LEFT))
	{
		transform_.rotate_.y -= 0.1f;
	}

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(transform_.rotate_.x, transform_.rotate_.y, 0);
	XMVECTOR vMove = { 0,0,0.1f };
	vMove = XMVector3TransformCoord(vMove, mRot);

	if (Input::IsKey(DIK_W))
	{
		vPos += vMove;
		XMStoreFloat3(&transform_.position_, vPos);
	}

	//XMVECTOR camPos = XMLoadFloat3(&transform_.position_);
	//XMFLOAT3 vTarget = transform_.position_;
	//vTarget = vTarget + camPos;

	//Camera::SetPosition(camPos);
	//Camera::SetTarget(vTarget);

	XMVECTOR vCam = { 0,5.0f,-10.0f,0 };
	vCam = XMVector3TransformCoord(vCam, mRot);
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, vPos + vCam);
	Camera::SetPosition(camPos);

	XMFLOAT3 camTarget = transform_.position_;
	Camera::SetTarget(camTarget);


	XMVECTOR vDir = XMVectorSubtract(XMLoadFloat3(&camPos), XMLoadFloat3(&camTarget));
	vDir = XMVector3Normalize(vDir);
	XMStoreFloat3(&camTarget, vDir);

	RayCastData data{
		{ camPos.x,camPos.y,camPos.z,1 },
		{camTarget.x,camPos.y,camPos.z,0},
		false,
		100
	};
	Model::RayCast(hModelColl_, data);
	if (data.isHit)
	{
		int i = 0;
		i++;
	}
}

void Stage::Draw()
{
	/*Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);*/

	for (int j = 0;j < ZSIZE;j++) {
		for (int i = 0;i < XSIZE;i++)
		{
			int type = BLOCK_TYPE::DEFAULT;
			Transform t;
			t.position_.x = i;
			t.position_.z = j;
			t.scale_ = { 1.0,1.0,1.0 };
			Model::SetTransform(hModel_[type], t);
			Model::Draw(hModel_[type]);

			// 適当にブロックを積み上げてるコード
			/*for (int k = 0; k < (int)(GetT(i, j).height); k++) {
				int type = (int)(GetT(i, j).type);
				Transform t;
				t.position_.x = i - 15 / 2.0;
				t.position_.z = j;
				t.position_.y = k - 15;
				t.scale_ = { 0.95, 0.95, 0.95 };
				Model::SetTransform(hModel_[type], t);
				Model::Draw(hModel_[type]);
			}*/ 
		}
	}

	//Transform t;
	//t.position_.x = 5;
	//t.position_.z = 5;
	//t.position_.y = 0;
	//t.scale_ = { 0.95, 0.95, 0.95 };
	//int type = BLOCK_TYPE::WATER;
	//Model::SetTransform(hModel_[type], t);
	//Model::Draw(hModel_[type]);

	//RayCastData rayData{
	//	{ 5.0f, 0.0f, 5.0f,1.0f },
	//	{ 0.0f,-1.0f, 0.0f,0.0f },
	//	false,
	//	100.0f
	//};
	//Model::RayCast(hModel_[type], rayData);
	//if (rayData.isHit)
	//{
	//	//MessageBoxA(NULL, "Hit", "Info", MB_OK);
	//}
}

void Stage::Release()
{
	KillMe();
}

void Stage::OnCollision(GameObject* pTarget)
{
	
}

BOOL Stage::localProck(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: //コントロールの操作
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL Stage::manuProck(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"デフォルト");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"レンガ");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"草地");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"砂地");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"水場");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_SETCURSEL, 0, 0);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_RADIO1:
			mode_ = 0;
			return TRUE;
		case IDC_RADIO2:
			mode_ = 1;
			return TRUE;
		case IDC_RADIO3:
			mode_ = 2;
			return TRUE;
		case IDC_COMBO1:
			select_ = (int)SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETCURSEL, 0, 0);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
