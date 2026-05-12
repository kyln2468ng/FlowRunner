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
	isWall_ = false;
	velocity_ = { 0.0f, 0.0f, 0.0f };
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
		
	XMVECTOR move = XMVectorZero();

	float inputX = 0.0f;
	float inputZ = 0.0f;

	if (Input::IsKey(DIK_W)) {
		inputZ += 1.0f;
	}
	if (Input::IsKey(DIK_S)) {
		inputZ -= 1.0f;
	}
	if (Input::IsKey(DIK_A)) {
		inputX -= 1.0f;
	}
	if (Input::IsKey(DIK_D)) {
		inputX += 1.0f;
	}

	if (inputX != 0.0f || inputZ != 0.0f) {
		float angle = atan2f(-inputX, inputZ);
		transform_.SetVectorRotation(XMFLOAT3(0, XMConvertToDegrees(angle), 0));
	}

	XMVECTOR forward = transform_.rotate_.Forward();
	XMVECTOR right = transform_.rotate_.Right();

	forward = XMVectorSetY(forward, 0.0f);
	right = XMVectorSetY(right, 0.0f);

	forward = XMVector3Normalize(forward);
	right = XMVector3Normalize(right);

	//if (Input::IsKey(DIK_W)) {
	//	move += forward;
	//}
	//if (Input::IsKey(DIK_S)) {
	//	move -= forward;
	//}
	//if (Input::IsKey(DIK_A)) {
	//	move -= right;
	//}
	//if (Input::IsKey(DIK_D)) {
	//	move += right;
	//}
	move = XMVectorSet(inputX, 0.0f, inputZ, 0.0f);

	/// プレイヤーから見たレイで壁を認識
	WallHitData WallData = DetectWall(vPos, forward, right);

	bool justJumped = false;
	if (Input::IsKeyDown(DIK_SPACE) && onGround_)// && coolTime_ < 0.0f)
	{
		velocity_.y = JumpV0;
		onGround_ = false;

		isWall_ = false;
		WallData.isHit = false;
		justJumped = true;
		/*bullet_ = (Bullet*)Instantiate<Bullet>(FindObject("PlayScene"));
		bullet_->SetPosition(transform_.position_);
		coolTime_ = nextTime;*/
		/*float jumpV0 = (1.0f - pow(1.0f - sin(3.1415926535 * JumpTime), distortion)) * JumpHeight;
		velocity_.y = abs(jumpV0);*/

	}

	if (!XMVector3Equal(move, XMVectorZero())) {
		move = XMVector3Normalize(move);
	}

	

	move *= param_.MOVE_SPEED;



	//if (WallData.isHit) {
	//	//壁に吸着する　////壁に吸着するパターンと重力のベクトルを壁方向に帰れる場所
	//	
	//	WallCollision(vPos, move, WallData);

	//	WallMove(move, WallData);

	//	WallCling(WallData);
	//}

	if (WallData.isHit && Input::IsKeyDown(DIK_S) && Input::IsKeyDown(DIK_SPACE)) {
		WallJump(WallData);
	}
	else if (WallData.isHit) {
		WallCollision(vPos, move, WallData);

		WallMove(move, WallData);

		WallCling(WallData);
	}
	else if (WallData.isHit == false) {
		velocity_.y -= param_.GRAVITY;
	}

	vPos += move;
	vPos += XMLoadFloat3(&velocity_);
	velocity_.x *= 0.9f;
	velocity_.z *= 0.9f;

	//if (isWall_) {
	//	WallCling(WallData);
	//}


	DirectX::XMStoreFloat3(&transform_.position_, vPos);

	//XMVECTOR camPos = XMLoadFloat3(&transform_.position_);
	//XMFLOAT3 vTarget = transform_.position_;
	//vTarget = vTarget + camPos;

	//Camera::SetPosition(camPos);
	//Camera::SetTarget(vTarget);

	
	//XMVECTOR camOffset = transform_.rotate_.Up() * 5.0f + (-transform_.rotate_.Forward() * 13.0f);
	XMVECTOR camOffset = XMVectorSet(0, 5, -13, 0);
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
	data.maxDist = playerHeight + fabs(velocity_.y) + 0.2f;

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
	Stage* st = (Stage*)FindObject("Stage");
	if (st && st->hitObject(data)) {
		if (data.isHit && data.dist <= data.maxDist) {
			if (velocity_.y <= 0.0f) {
				groundY = data.hitPos.y;
				isGround = true;
			}
		}
	}

	//velocity_.y -= param_.GRAVITY;
	// 重力はすでに velocity に反映済みとする
	float nextY = transform_.position_.y + velocity_.y;
	float nextFoot = nextY - playerHeight;

	// 落下中 & 地面を超えるなら
	if (velocity_.y < 0.0f && nextFoot <= groundY && isGround)
	{
		// 着地
		transform_.position_.y = groundY + playerHeight;
		velocity_.y = 0.0f;
		onGround_ = true;
	}
	else
	{
		transform_.position_.y = nextY;
		onGround_ = false;
	}

	/// 値検証用変数 ///
	//drawDist = closeDist;

	//transform_.position_.y += velocity_;
	//m = move;

	
	
}

WallHitData Player::DetectWall(const XMVECTOR& vPos, const XMVECTOR& move, const XMVECTOR& right)
{
	WallHitData result;

	Stage* st = (Stage*)FindObject("Stage");

	//移動方向
	XMVECTOR moveDir = move;

	//if (!XMVector3Equal(move, XMVectorZero())) {
	//	moveDir = XMVector3Normalize(move);
	//}
	//else {
	//	moveDir = transform_.rotate_.Forward();
	//}

	if (isWall_) {
		moveDir = -XMLoadFloat3(&wallNormal_);
	}
	else {
		moveDir = transform_.rotate_.Forward();
	}

	moveDir = XMVectorSetY(moveDir, 0.0f);

	if (!XMVector3Equal(moveDir, XMVectorZero())) {
		moveDir = XMVector3Normalize(moveDir);

	}

	float playerRadius = 0.5f;

	std::vector<XMVECTOR> offsets = {
		XMVectorZero(),
		right * playerRadius,
		-right * playerRadius
	};

	for (auto& offset : offsets) {

		// レイ方向に沿って前に出す
		XMVECTOR rayStartVec = vPos + offset + moveDir * playerRadius;

		RayCastData wallRay = {
			{ XMVectorGetX(rayStartVec), XMVectorGetY(rayStartVec), XMVectorGetZ(rayStartVec) , 1 },
			{ XMVectorGetX(moveDir), XMVectorGetY(moveDir), XMVectorGetZ(moveDir), 0 }
		};
		wallRay.maxDist = 0.6f;

		if (st && st->hitObject(wallRay) && wallRay.isHit) {

			XMVECTOR wallNormal = XMLoadFloat3(&wallRay.hitNormal);
			wallNormal = XMVectorSetY(wallNormal, 0.0f);

			if (!XMVector3Equal(wallNormal, XMVectorZero())) {
				wallNormal = XMVector3Normalize(wallNormal);
			}

			float dot = XMVectorGetX(XMVector3Dot(moveDir, wallNormal));

			if (dot < 0.0f) {
				if (wallRay.dist < result.dist) {
					result.isHit = true;
					result.dist = wallRay.dist;
					result.normal = wallNormal;
					result.hitPos = XMLoadFloat3(&wallRay.hitPos);
					velocity_.y = 0.0f;
				}
			}

		}
	}

	if (result.isHit) {
		XMStoreFloat3(&wallNormal_, result.normal);
	}

	return result;
}

void Player::WallCling(const WallHitData& wall)
{
	float offset = 0.05f;
	XMVECTOR target = wall.hitPos - wall.normal * offset;

	XMVECTOR current = XMLoadFloat3(&transform_.position_);
	//XMVECTOR newPos = XMVectorLerp(current, target, 0.1f);
	//XMVECTOR newPos = hitPos - normal * offset;
	float dist = wall.dist - offset;
	current -= wall.normal * dist;
	DirectX::XMStoreFloat3(&transform_.position_, current);
	//velocity_ = 0.0f;
}

void Player::WallCollision(XMVECTOR& vPos, XMVECTOR& move, const WallHitData& wall)
{
	float dot = XMVectorGetX(XMVector3Dot(move, wall.normal));

	if (dot < 0.0f) {
		move -= wall.normal * dot;
	}

	float playerRadius = 0.5f;
	float penetration = playerRadius - wall.dist;

	if (penetration > 0.0f) {
		vPos += wall.normal * penetration;
	}
}

void Player::WallMove(XMVECTOR& move, const WallHitData& wall)
{
	XMVECTOR wallRight = XMVector3Cross(wall.normal, XMVectorSet(0, 1, 0, 0));

	wallRight =	XMVector3Normalize(wallRight);

	XMVECTOR wallUp = XMVector3Cross(wallRight, wall.normal);

	wallUp = XMVector3Normalize(wallUp);

	XMVECTOR wallMove =	XMVectorZero();

	if (Input::IsKey(DIK_W)) {
		wallMove += wallUp;
	}

	if (Input::IsKey(DIK_S)) {
		wallMove -= wallUp;
	}

	if (Input::IsKey(DIK_A)) {
		wallMove -= wallRight;
	}

	if (Input::IsKey(DIK_D)) {
		wallMove += wallRight;
	}

	if (Input::IsKeyDown(DIK_SPACE)) {
		XMVECTOR jumpDir = wall.normal + XMVectorSet(0, 1.0f, 0, 0);
		jumpDir = XMVector3Normalize(jumpDir);
		jumpDir = XMVectorScale(jumpDir, JumpV0);
		XMStoreFloat3(&velocity_, jumpDir);
	}

	if (!XMVector3Equal(
		wallMove,
		XMVectorZero()))
	{
		wallMove =
			XMVector3Normalize(wallMove);
	}

	move = wallMove * param_.MOVE_SPEED;
}

void Player::WallJump(const WallHitData& wall)
{
	XMVECTOR jumpDir = wall.normal + XMVectorSet(0, 1.0f, 0, 0);
	jumpDir = XMVector3Normalize(jumpDir);
	jumpDir = XMVectorScale(jumpDir, JumpV0);
	XMStoreFloat3(&velocity_, jumpDir);	
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



