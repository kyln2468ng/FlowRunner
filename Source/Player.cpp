#include "Player.h"
#include "ChildOden.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/SphereCollider.h"
#include "Bullet.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "Stage.h"

namespace
{
	float coolTime_ = 0.0f;
	const float nextTime = 0.5f;
	const float deltatime_ = 0.016;
}

Player::Player(GameObject* parent)
	:GameObject(parent, "Player"), hModel_(-1)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	//pFbx_ = new Fbx;
	//pFbx_がnullptrじゃなかった時のチェックあったほういい
	//pFbx_->Load("OdenA.fbx");
	hModel_ = Model::Load("OdenA.fbx");
	assert(hModel_ >= 0);
	transform_.scale_.x = 1.0f;
	transform_.scale_.y = 1.0f;
	transform_.scale_.z = 1.0f;

	transform_.position_ = { -2.0f,10.0f,3.0f };

	//子オブジェクトにChildOdenを追加する
	pRChildOden = (ChildOden*)Instantiate<ChildOden>(this);
	pLChildOden = (ChildOden*)Instantiate<ChildOden>(this);
	pRChildOden->SetPosition(2.0f, 1.0f, 0.0f);
	pLChildOden->SetPosition(-2.0f, 1.0f, 0.0f);

	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);
}

void Player::Update()
{
	//transform_.rotate_.y += 1.0f;
	//transform_.position_.y -= 0.1f;//////
	/*if (transform_.rotate_.y >= 720.0f)
	{
		KillMe();
	}*/
	/*if (Input::IsKey(DIK_W))
	{
		transform_.position_.z += 0.1;
	}*/
	if (Input::IsKey(DIK_S))
	{
		transform_.position_.z -= 0.1f;
	}
	if (Input::IsKey(DIK_A))
	{
		transform_.position_.x -= 0.1f;
	}
	if (Input::IsKey(DIK_D))
	{
		transform_.position_.x += 0.1f;
	}
	if (Input::IsKey(DIK_E))
	{
		transform_.position_.y -= 0.1f;
	}

	coolTime_ -= deltatime_;
	if (Input::IsKeyDown(DIK_SPACE) && coolTime_ < 0.0f)
	{
		bullet_ = (Bullet*)Instantiate<Bullet>(FindObject("PlayScene"));
		bullet_->SetPosition(transform_.position_);
		coolTime_ = nextTime;
	}

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

	XMFLOAT3 pos = transform_.position_;
	float playerHeight = 2.05f;//←ここ少し調整した
	RayCastData data = {
		{ pos.x, pos.y - playerHeight, pos.z, 1},
		{0.0f,-10.0f,0.0f,0.0f}
	};
	//data.maxDist = playerHeight + 1.5f;//←ここ少し調整した

	Stage* st = (Stage*)FindObject("Stage");
	//int hStageModel = st->GetModel();

	//Model::RayCast(hStageModel, data);
	//if (data.isHit) {
	//	transform_.position_.y = data.hitPos.y;
	//}
	/*if (data.isHit && data.dist > 0.01f && data.dist <= playerHeight + 0.2f)
	{
		transform_.position_.y = data.hitPos.y + playerHeight;
	}*/
	if (st && st->hitObject(data)) {
		//transform_.position_.y = data.hitPos.y + playerHeight;
		int i = 0;
		i++;
	}
}

void Player::Draw()
{
	/*if (pFbx_)
	{
		pFbx_->Draw(transform_);
	}*/
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Player::Release()
{
	KillMe();
}

void Player::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() == "Enemy")
	{
		this->Release();
		SceneManager* sceneOb = (SceneManager*)FindObject("SceneManager");
		sceneOb->ChangeScene(SCENE_ID_GAMEOVER);
	}
}

