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
	//float gravity_ = 0.001f;
	float JumpTime = 1.0f;
	float distortion = 1.0f;
	//float JumpHeight = 3.0f;
	float JumpV0 = 0;

	float drawDist = 0;
	XMVECTOR m;
}

Player::Player(GameObject* parent)
	:GameObject(parent, "Player"), hModel_(-1),param_(GetPlayerParamConfig())
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
	hModel_ = Model::Load("BoxGrass.fbx");//おでんじゃなくしたら判定取れてた
	assert(hModel_ >= 0);
	transform_.scale_.x = 1.0f;
	transform_.scale_.y = 1.0f;
	transform_.scale_.z = 1.0f;

	transform_.position_ = { 0.0f,-1.0,3.0f };

	//子オブジェクトにChildOdenを追加する
	//pRChildOden = (ChildOden*)Instantiate<ChildOden>(this);
	//pLChildOden = (ChildOden*)Instantiate<ChildOden>(this);
	//pRChildOden->SetPosition(2.0f, 1.0f, 0.0f);
	//pLChildOden->SetPosition(-2.0f, 1.0f, 0.0f);

	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);


	JumpV0 = sqrtf(2.0f * param_.GRAVITY * param_.JUMP_HEIGHT);
	onGround_ = false;
	velocityY = 0.0f;
	isWall_ = false;
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

	//coolTime_ -= deltatime_;
	bool justJumped = false;
	if (Input::IsKeyDown(DIK_SPACE) && onGround_)// && coolTime_ < 0.0f)
	{
		velocityY = JumpV0;
		onGround_ = false;

		isWall_ = false;
		justJumped = true;
		/*bullet_ = (Bullet*)Instantiate<Bullet>(FindObject("PlayScene"));
		bullet_->SetPosition(transform_.position_);
		coolTime_ = nextTime;*/
		/*float jumpV0 = (1.0f - pow(1.0f - sin(3.1415926535 * JumpTime), distortion)) * JumpHeight;
		velocityY = abs(jumpV0);*/

	}


	//視点移動をする
	if (Input::IsKey(DIK_RIGHT)) {
		//transform_.rotate_.y += param_.MOVE_SPEED;
		transform_.SetVectorRotation(XMFLOAT3(0, 10, 0));
	}
	if (Input::IsKey(DIK_LEFT)) {
		//transform_.rotate_.y -= param_.MOVE_SPEED;
		transform_.SetVectorRotation(XMFLOAT3(0, 10, 0));
	}

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	//XMMATRIX mRot = XMMatrixRotationRollPitchYaw(transform_.rotate_.x, transform_.rotate_.y, 0);
	XMMATRIX mRot = XMMatrixRotationQuaternion(transform_.rotate_.quaternion_);

	XMVECTOR forward = transform_.rotate_.Forward();
	XMVECTOR right = transform_.rotate_.Right();

	forward = XMVectorSetY(forward,0.0f);
	right = XMVectorSetY(right, 0.0f);

	forward = XMVector3Normalize(forward);
	right = XMVector3Normalize(right);

	XMVECTOR move = XMVectorZero();

	if (Input::IsKey(DIK_W)) {
		move += forward;
	}
	if (Input::IsKey(DIK_S)) {
		move -= forward;
	}
	if (Input::IsKey(DIK_A)) {
		move -= right;
	}
	if (Input::IsKey(DIK_D)) {
		move += right;
	}

	if (!XMVector3Equal(move, XMVectorZero())) {
		move = XMVector3Normalize(move);
	}

	move *= param_.MOVE_SPEED;

	/// プレイヤーから見たレイで壁を認識
	XMVECTOR posVec = XMLoadFloat3(&transform_.position_);
	bool hit = false;
	float closeDist = FLT_MAX;
	XMVECTOR bestNormal = XMVectorZero();
	XMVECTOR bestHitPos = XMVectorZero();

	XMVECTOR wallNormal = XMVectorZero();

	Stage* st = (Stage*)FindObject("Stage");

	XMVECTOR moveDir = move;
	if (!XMVector3Equal(move, XMVectorZero())) {
		moveDir = XMVector3Normalize(move);
	}
	else {
		moveDir = transform_.rotate_.Forward();
	}
	float playerRadius = 0.5f;

	std::vector<XMVECTOR> offsets = {
		XMVectorZero(),
		right * playerRadius,
		-right * playerRadius
	};

	for (auto& offset : offsets) {

		// レイ方向に沿って前に出す
		XMVECTOR rayStartVec = vPos + offset+ moveDir * playerRadius;

		RayCastData wallRay = {
			{ XMVectorGetX(rayStartVec), XMVectorGetY(rayStartVec), XMVectorGetZ(rayStartVec) , 1 },
			{ XMVectorGetX(moveDir), XMVectorGetY(moveDir), XMVectorGetZ(moveDir), 0 }
		};
		wallRay.maxDist = 0.6f;
		//moveDir = XMVector3Normalize(moveDir);
		//wallRay.maxDist = XMVectorGetX(XMVector3Length(move)) + 0.1f;


		if (st && st->hitObject(wallRay) && wallRay.isHit) {
			hit = true;

			wallNormal = XMLoadFloat3(&wallRay.hitNormal);
			wallNormal = XMVectorSetY(wallNormal, 0.0f);
			if (!XMVector3Equal(wallNormal, XMVectorZero())) {
				wallNormal = XMVector3Normalize(wallNormal);
			}

			if (wallRay.dist < closeDist) {
				closeDist = wallRay.dist;

				bestNormal = XMLoadFloat3(&wallRay.hitNormal);
				bestHitPos = XMLoadFloat3(&wallRay.hitPos);
			}

			float dot = XMVectorGetX(XMVector3Dot(moveDir, wallNormal));

			//if (dot < 0.0f) {
				/////あとで使うかも
				//move -= wallNormal * dot;
				//move += wallNormal * -0.01f * XMVectorGetX(XMVector3Length(move));
			//}

			if (!justJumped) {
				if (wallRay.dist < 0.5f && dot < 0.0f) {
					isWall_ = true;
				}
				else if (wallRay.dist > 0.6f) {
					isWall_ = false;
				}
				//else if (wallRay.dist < 0.5f) {
				//	isWall_ = true;
				//}
			}
		}
	}

	XMVECTOR hitPos = bestHitPos;
	XMVECTOR normal = bestNormal;

	if (isWall_) {
		//壁に吸着する　////壁に吸着するパターンと重力のベクトルを壁方向に帰れる場所

		//WallCling(hitPos, normal);
		
		JudgeWall(vPos, move, normal, closeDist);

		XMVECTOR wallRight = XMVector3Cross(normal, XMVectorSet(0, 1, 0, 0));
		wallRight = XMVector3Normalize(wallRight);

		XMVECTOR moving = XMVectorZero();

		if (Input::IsKeyDown(DIK_A)) {
			moving -= wallRight;
		}
		if (Input::IsKeyDown(DIK_D)) {
			moving += wallRight;
		}
		move = moving * param_.MOVE_SPEED;

		//WallMove(move, normal); // 値送れない。後で修正
		//WallSlide(move, normal);
	}
	//else {
	//	// 最後に移動
	//	vPos += move;
	//}

	vPos += move;

	if (isWall_) {
		WallCling(hitPos, normal);
	}

	DirectX::XMStoreFloat3(&transform_.position_, vPos);

	//XMVECTOR camPos = XMLoadFloat3(&transform_.position_);
	//XMFLOAT3 vTarget = transform_.position_;
	//vTarget = vTarget + camPos;

	//Camera::SetPosition(camPos);
	//Camera::SetTarget(vTarget);

	{
		XMVECTOR camOffset = transform_.rotate_.Up() * 5.0f + (-transform_.rotate_.Forward() * 13.0f);

		XMFLOAT3 camPos;
		DirectX::XMStoreFloat3(&camPos, vPos + camOffset);

		Camera::SetPosition(camPos);
		Camera::SetTarget(transform_.position_);

		//transform_.position_.y -= gravity_;

		XMFLOAT3 pos = transform_.position_;
		float playerHeight = 1.0f;
		RayCastData data = {
			{ pos.x, pos.y, pos.z, 1},
			{0.0f,-1.0f,0.0f,0.0f}
		};
		data.maxDist = playerHeight + fabs(velocityY) + 0.2f;

		//int hStageModel = st->GetModel();

		//Model::RayCast(hStageModel, data);
		//if (data.isHit) {
		//	transform_.position_.y = data.hitPos.y;
		//}
		/*if (data.isHit && data.dist > 0.01f && data.dist <= playerHeight + 0.2f)
		{
			transform_.position_.y = data.hitPos.y + playerHeight;
		}*/


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

		velocityY -= param_.GRAVITY;
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

		drawDist = closeDist;

		//transform_.position_.y += velocityY;
		//m = move;

	}
	
}

void Player::WallCling(DirectX::XMVECTOR hitPos_, DirectX::XMVECTOR normal_)
{
	float offset = 0.05f;
	XMVECTOR target = hitPos_ - normal_ * offset;

	XMVECTOR current = XMLoadFloat3(&transform_.position_);
	XMVECTOR newPos = XMVectorLerp(current, target, 0.1f);
	//XMVECTOR newPos = hitPos - normal * offset;
	DirectX::XMStoreFloat3(&transform_.position_, newPos);
	velocityY = 0.0f;
}

void Player::JudgeWall(XMVECTOR& vPos, XMVECTOR& move, const XMVECTOR& normal, float dist)
{
	float playerRadius = 0.5f;
	float penetration = playerRadius - dist;

	if (penetration > 0.0f) {
		vPos += normal * penetration;
	}

	//壁スライド(めり込み防止）
	float dot = XMVectorGetX(XMVector3Dot(move, normal));

	if (dot < 0.0f) {
		move -= normal * dot;
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
	
	char buf[128];
	//sprintf_s(buf, "dist: %.2f\n", drawDist);
	
	XMFLOAT3 mx;
	XMStoreFloat3(&mx, m);
	
	sprintf_s(buf, "move: %.2f %.2f %.2f\n", mx.x, mx.y, mx.z);

	OutputDebugStringA(buf);

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



