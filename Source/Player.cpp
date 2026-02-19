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
	float gravity_ = 0.001f;
	float JumpTime = 1.0f;
	float distortion = 1.0f;
	float JumpHeight = 3.0f;
	float JumpV0 = 0;
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

	transform_.position_ = { 0.0f,-1.0,3.0f };

	//子オブジェクトにChildOdenを追加する
	pRChildOden = (ChildOden*)Instantiate<ChildOden>(this);
	pLChildOden = (ChildOden*)Instantiate<ChildOden>(this);
	pRChildOden->SetPosition(2.0f, 1.0f, 0.0f);
	pLChildOden->SetPosition(-2.0f, 1.0f, 0.0f);

	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);


	JumpV0 = sqrtf(2.0f * gravity_ * JumpHeight);
	onGround_ = false;
	velocityY = 0.0f;
}

void Player::Update()
{
	//transform_.rotate_.y += 1.0f;
	//transform_.position_.y -= 0.1f;
	/*if (transform_.rotate_.y >= 720.0f)
	{
		KillMe();
	}*/
	/*if (Input::IsKey(DIK_W))
	{
		transform_.position_.z += 0.1;
	}*/

	//onGround_ = false;

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

	//coolTime_ -= deltatime_;
	if (Input::IsKeyDown(DIK_SPACE) && onGround_)// && coolTime_ < 0.0f)
	{
		velocityY = JumpV0;
		onGround_ = false;


		/*bullet_ = (Bullet*)Instantiate<Bullet>(FindObject("PlayScene"));
		bullet_->SetPosition(transform_.position_);
		coolTime_ = nextTime;*/
		/*float jumpV0 = (1.0f - pow(1.0f - sin(3.1415926535 * JumpTime), distortion)) * JumpHeight;
		velocityY = abs(jumpV0);*/

	}

	velocityY -= gravity_;
	transform_.position_.y += velocityY;

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

	XMVECTOR vCam = { 0,5.0f,-13.0f,0 };
	vCam = XMVector3TransformCoord(vCam, mRot);
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, vPos + vCam);
	Camera::SetPosition(camPos);

	XMFLOAT3 camTarget = transform_.position_;
	Camera::SetTarget(camTarget);


	XMVECTOR vDir = XMVectorSubtract(XMLoadFloat3(&camPos), XMLoadFloat3(&camTarget));
	vDir = XMVector3Normalize(vDir);
	XMStoreFloat3(&camTarget, vDir);

	//transform_.position_.y -= gravity_;

	XMFLOAT3 pos = transform_.position_;
	float playerHeight = 1.0f;
	RayCastData data = {
		{ pos.x, pos.y, pos.z, 1},
		{0.0f,-1.0f,0.0f,0.0f}
	};
	data.maxDist = playerHeight + fabs(velocityY) + 0.2f;
	
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


	////着地回りや物理部分は一旦後に持っていく、先にステージの生成を作ってその後やっていく感じにする////
	float groundY = 0.0f;
	bool isGround = false;

	if (st && st->hitObject(data)) {
		if (data.isHit && data.dist <= data.maxDist) {
			if (velocityY <= 0.0f) {
				groundY = data.hitPos.y;
				isGround = true;
			}
		}
	}


	// 重力はすでに velocity に反映済みとする
	float nextY = transform_.position_.y + velocityY;
	float nextFoot = nextY - playerHeight;

	// 落下中 & 地面を超えるなら
	if (velocityY < 0.0f && nextFoot <= groundY && isGround)
	{
		// 着地
		transform_.position_.y = groundY + playerHeight;
		velocityY = 0.0f;
		onGround_ = true;
	}
	else
	{
		transform_.position_.y = nextY;
		onGround_ = false;
	}

	//if (onGround_) {
	//	float groundY = data.hitPos.y;
	///*	if (velocityY <= 0.0f) {
	//		transform_.position_.y = groundY + playerHeight;
	//		velocityY = 0.0f;
	//		gravity_ = 0.0f;
	//	}*/
	//	gravity_ = 0.0f;
	//	velocityY = 0.0f;
	//	if (transform_.position_.y - playerHeight-velocityY <= groundY) {
	//		transform_.position_.y = groundY + playerHeight;
	//		gravity_ = 0.0f;
	//	}
	//	else if (transform_.position_.y < -6.0) {
	//		transform_.position_.y = groundY + playerHeight;
	//		gravity_ = 0.0f;
	//		velocityY = 0.0f;
	//	}

	//}
	//else
	//	gravity_ = 0.1f;

	

	//if (onGround_) {
	//	transform_.position_.y = data.hitPos.y + playerHeight;
	//	gravity_ = 0.0f;
	//}
	//else
	//	gravity_ = 0.1f;


	

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

