#include "Player.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "Stage.h"
#include "../Engine/CsvReader.h"


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

	//
	//挙動
	//アニメーション
	//
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
	hModel_ = Model::Load("model/baseModel.fbx");
	assert(hModel_ >= 0);
	transform_.scale_.x = 0.5f;
	transform_.scale_.y = 0.5f;
	transform_.scale_.z = 0.5f;

	transform_.position_ = { 0.0f,-1.0,3.0f };

	SphereCollider* col = new SphereCollider(0.5f);
	AddCollider(col);


	JumpV0 = sqrtf(2.0f * param_.GRAVITY * param_.JUMP_HEIGHT);
	onGround_ = false;
	justClimb_ = false;
	isWall_ = false;
	velocity_ = { 0.0f, 0.0f, 0.0f };

	LoadAnimation();
	SetState(AnimationState::IDLE);
	Camera::SetMouseControl(true);

	state_ = PlayerState::IDLE;
}

void Player::Update()
{
	UpdateAnimation();

	switch (state_)
	{
	case IDLE:
		UpdateIdle();
		break;
	case WALK:
		UpdateWalk();
		break;
	case JUMP:
		UpdateJump();
		break;
	case FALL:
		UpdateFall();
		break;
	case WALL:
		UpdateWall();
		break;
	case WALL_JUMP:
		UpdateWallJump();
		break;
	}
}

WallHitData Player::DetectWall(const XMVECTOR& vPos, const XMVECTOR& move, const XMVECTOR& right)
{
	WallHitData result;

	Stage* st = (Stage*)FindObject("Stage");

	//移動方向
	XMVECTOR moveDir = move;

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

	float playerRadius = 0.1f;
	float playerHeight = 1.8f;

	std::vector<XMVECTOR> offsets = {
		// 足元
		XMVectorZero(),
		right * playerRadius,
		-right * playerRadius,

		// 中央
		XMVectorSet(0, playerHeight * 0.5f, 0, 0),
		right* playerRadius + XMVectorSet(0, playerHeight * 0.5f, 0, 0),
		-right * playerRadius + XMVectorSet(0, playerHeight * 0.5f, 0, 0),
		
		// 上
		XMVectorSet(0, playerHeight * 0.9f, 0, 0),
		right* playerRadius + XMVectorSet(0, playerHeight * 0.9f, 0, 0),
		-right * playerRadius + XMVectorSet(0, playerHeight * 0.9f, 0, 0)
	};

	for (auto& offset : offsets) {

		// レイ方向に沿って前に出す
		XMVECTOR rayStartVec = vPos + offset + moveDir * playerRadius;

		RayCastData wallRay = {
			{ XMVectorGetX(rayStartVec), XMVectorGetY(rayStartVec), XMVectorGetZ(rayStartVec) , 1 },
			{ XMVectorGetX(moveDir), XMVectorGetY(moveDir), XMVectorGetZ(moveDir), 0 }
		};
		wallRay.maxDist = 0.5f;

		if (st && st->hitObject(wallRay,hModel_) && wallRay.isHit) {

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
	float dist = wall.dist - offset;
	current -= wall.normal * dist;
	DirectX::XMStoreFloat3(&transform_.position_, current);
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

	if (!XMVector3Equal(wallMove, XMVectorZero())) {
		wallMove =	XMVector3Normalize(wallMove);
	}

	move = wallMove *  param_.MOVE_SPEED;
}

void Player::WallJump(const WallHitData& wall)
{
	XMVECTOR jumpDir = wall.normal + XMVectorSet(0, 1.0f, 0, 0);
	jumpDir = XMVector3Normalize(jumpDir);
	jumpDir = XMVectorScale(jumpDir, JumpV0);
	XMStoreFloat3(&velocity_, jumpDir);	
}

bool Player::IsWallTop(const XMVECTOR& vPos, const XMVECTOR& forward, float& groundY)
{
	Stage* st = (Stage*)FindObject("Stage");

	if (!st) {
		return false;
	}

	// 頭より少し上からレイを出す
	XMVECTOR start = vPos + forward * 0.3f;
	start += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	RayCastData data = {
		{
			XMVectorGetX(start),
			XMVectorGetY(start),
			XMVectorGetZ(start),
			1.0f
		},
		{ 0.0f, -1.0f, 0.0f, 0.0f }
	};

	data.maxDist = 2.0f;


	if (!st->hitObject(data, hModel_) || !data.isHit)
	{
		return false;
	}

	return true;
}

void Player::UpdateIdle()
{
	if (onGround_ == false) {
		state_ = FALL;
	}

	if (Input::IsKeyDown(DIK_SPACE) && onGround_) {
		velocity_.y = JumpV0;
		onGround_ = false;
		isWall_ = false;
		state_ = JUMP;
		return;
	}

	if (Input::IsKey(DIK_W) ||	Input::IsKey(DIK_A) ||	Input::IsKey(DIK_S) ||	Input::IsKey(DIK_D)) {
		SetState(AnimationState::WALK);
		state_ = WALK;
	}
}

void Player::UpdateWalk()
{
	if (!Input::IsKey(DIK_W) &&	!Input::IsKey(DIK_A) &&	!Input::IsKey(DIK_S) &&	!Input::IsKey(DIK_D)) {
		SetState(AnimationState::IDLE);
		state_ = IDLE;
	}
	

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	XMVECTOR cameraForward = Camera::GetForward();
	XMVECTOR cameraRight = Camera::GetRight();

	XMVECTOR move = XMVectorZero();

	float inputX = 0.0f;
	float inputZ = 0.0f;

	if (Input::IsKey(DIK_W)) {
		move += cameraForward;
	}
	if (Input::IsKey(DIK_S)) {
		move -= cameraForward;
	}
	if (Input::IsKey(DIK_A)) {
		move -= cameraRight;
	}
	if (Input::IsKey(DIK_D)) {
		move += cameraRight;
	}

	Stage* st = (Stage*)FindObject("Stage");
	if (st && st->IsEdhitorMode()) {
		velocity_ = {};
		isWall_ = false;
		return;
	}

	//
	float groundDist = 0.1f;

	RayCastData data = {
		{
			transform_.position_.x,
			transform_.position_.y,
			transform_.position_.z,
			1.0f
		},
		{ 0.0f, -1.0f, 0.0f, 0.0f }
	};

	data.maxDist = groundDist;

	if (justClimb_)	{
		justClimb_ = false;
	}
	else {
		// 下向きレイによる通常の落下判定
		if (!st->hitObject(data, hModel_) || !data.isHit)
		{
			state_ = FALL;
			onGround_ = false;
			return;
		}
	}

	// プレイヤーの向きを入力方向に合わせる
	bool isMove = !XMVector3Equal(move, XMVectorZero());
	if (isMove) {
		// 移動方向を正規化
		move = XMVector3Normalize(move);

		// 移動方向にプレイヤーの向きを合わせる
		float moveX = XMVectorGetX(move);
		float moveZ = XMVectorGetZ(move);

		float angle = atan2f(moveX, moveZ);

		transform_.SetVectorRotation(XMFLOAT3(0, XMConvertToDegrees(angle), 0));
	}

	XMVECTOR forward = transform_.rotate_.Forward();
	XMVECTOR right = transform_.rotate_.Right();

	forward = XMVectorSetY(forward, 0.0f);
	right = XMVectorSetY(right, 0.0f);

	forward = XMVector3Normalize(forward);
	right = XMVector3Normalize(right);

	// プレイヤーから見たレイで壁を認識
	WallHitData wallData = DetectWall(vPos, forward, right);

	if (!XMVector3Equal(move, XMVectorZero())) {
		move = XMVector3Normalize(move);
	}

	move *= param_.MOVE_SPEED;

	if (wallData.isHit) {
		isWall_ = true;
		state_ = WALL;
		return;
	}

	vPos += move;
	vPos += XMLoadFloat3(&velocity_);

	DirectX::XMStoreFloat3(&transform_.position_, vPos);

	Camera::SetTarget(transform_.position_);

	if (Input::IsKeyDown(DIK_SPACE) && onGround_) {
		velocity_.x = XMVectorGetX(move) * 1.5f;
		velocity_.z = XMVectorGetZ(move) * 1.5f;
		velocity_.y = JumpV0;
		onGround_ = false;

		isWall_ = false;
		wallData.isHit = false;

		state_ = JUMP;
	}
}

void Player::UpdateJump()
{
	velocity_.y -= param_.GRAVITY;

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	WallHitData wallData = DetectWall(
		vPos,
		transform_.rotate_.Forward(),
		transform_.rotate_.Right()
	);

	if (wallData.isHit) {
		isWall_ = true;
		state_ = WALL;
		return;
	}

	vPos += XMLoadFloat3(&velocity_);

	DirectX::XMStoreFloat3(&transform_.position_, vPos);

	Camera::SetTarget(transform_.position_);

	if (velocity_.y <= 0.0f) {
		state_ = FALL;
	}
}

void Player::UpdateFall()
{
	Stage* st = (Stage*)FindObject("Stage");

	XMFLOAT3 pos = transform_.position_;
	float groundDist = 0.01f;

	RayCastData data = {
		{ pos.x, pos.y, pos.z, 1 },
		{ 0.0f, -1.0f, 0.0f, 0.0f }
	};

	data.maxDist = groundDist + fabs(velocity_.y) + 0.1f;

	float groundY = 0.0f;
	bool isGround = false;

	if (st && st->hitObject(data, hModel_)) {
		if (data.isHit && data.dist <= data.maxDist) {
			if (velocity_.y <= 0.0f) {
				groundY = data.hitPos.y;
				isGround = true;
			}
		}
	}

	velocity_.y -= param_.GRAVITY;

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	// X・Y・Zすべて移動
	vPos += XMLoadFloat3(&velocity_);

	float nextY = XMVectorGetY(vPos);
	float nextFoot = nextY - groundDist;

	if (velocity_.y < 0.0f && nextFoot <= groundY && isGround) {
		XMStoreFloat3(&transform_.position_, vPos);

		transform_.position_.y = groundY + groundDist;

		velocity_.x = 0.0f;
		velocity_.y = 0.0f;
		velocity_.z = 0.0f;

		onGround_ = true;

		state_ = IDLE;
		SetState(AnimationState::IDLE);
	}
	else {
		XMStoreFloat3(&transform_.position_, vPos);
		onGround_ = false;
	}

	Camera::SetTarget(transform_.position_);
}

void Player::UpdateWall()
{
	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	XMVECTOR move = XMVectorZero();

	XMVECTOR forward = transform_.rotate_.Forward();
	XMVECTOR right = transform_.rotate_.Right();

	forward = XMVectorSetY(forward, 0.0f);
	right = XMVectorSetY(right, 0.0f);
	
	forward = XMVector3Normalize(forward);
	right = XMVector3Normalize(right);

	// プレイヤーから見たレイで壁を認識
	WallHitData wallData = DetectWall(vPos, forward, right);

	isWall_ = true;

	if (!wallData.isHit) {
		float groundY = 0.0f;
		if (IsWallTop(vPos, forward, groundY))
		{
			// 壁の上に乗る
			transform_.position_.y = groundY + 0.1f;
			velocity_.y = 0.0f;
			isWall_ = false;
			justClimb_ = true;

			state_ = IDLE;
			SetState(AnimationState::IDLE);
			return;
		}

		//壁から離れたらFALL
		isWall_ = false;
		state_ = FALL;
		return;
	}


	// 壁ジャンプ
 	if (Input::IsKeyDown(DIK_S) && Input::IsKeyDown(DIK_SPACE) || Input::IsMouseButtonDown(0)) {
 		WallJump(wallData);

		isWall_ = false;
		onGround_ = false;

		state_ = WALL_JUMP;
		return;
	}

	// 壁との衝突
	WallCollision(vPos, move, wallData);

	// 壁に沿って移動
	WallMove(move, wallData);

	// 壁に張り付く
	WallCling(wallData);

	vPos += move;

	DirectX::XMStoreFloat3(&transform_.position_, vPos);

	Camera::SetTarget(transform_.position_);
}

void Player::UpdateWallJump()
{
	velocity_.y -= param_.GRAVITY;

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

	vPos += XMLoadFloat3(&velocity_);

	DirectX::XMStoreFloat3(&transform_.position_, vPos);

	Camera::SetTarget(transform_.position_);

	if (velocity_.y <= 0.0f) {
		state_ = FALL;
	}
}

void Player::LoadAnimation()
{
	AddAnimation(AnimationState::IDLE, "model/baseModel.fbx");
	AddAnimation(AnimationState::WALK, "model/Walking.fbx");

	LoadAnimData("Assets/model/AnimationData.csv");
}

void Player::AddAnimation(AnimationState state, const std::string& animPath) {
	hModel_ = Model::Load(animPath);
	animData_[state].animPath = hModel_;
}

void Player::LoadAnimData(const std::string& FilePath)
{
	CsvReader csv(FilePath);
	for (int i = 0; i < csv.GetLines();i++) {
		
		AnimationState state = StringToState(csv.GetString(i, 0));

		if (state == AnimationState::STATE_MAX)
		{
			OutputDebugStringA("Unknown animation state.\n");
			continue;
		}

		if (auto it = animData_.find(state); it != animData_.end()) {

			it->second.startFrame = csv.GetInt(i, 1);
			it->second.endFrame = csv.GetInt(i, 2);
			it->second.speed = csv.GetFloat(i, 3);
			it->second.loop = csv.GetInt(i, 4) != 0;		
		}
		else {
			OutputDebugStringA("Animation state not found.\n");
		}
	}
}

void Player::UpdateAnimation()
{
	if (!currentAnimData_) 
		return;

	float animSpeed = currentAnimData_->speed;

	if (currentState_ == AnimationState::WALK) {
		animSpeed = GetWalkAnimSpeed();
	}

	int prevFrame = (int)currentFrame_;

	// アニメーションを進める
	currentFrame_ += animSpeed;

	bool isLoop = false;

	if (currentAnimData_->loop)	{
		if (currentFrame_ > currentAnimData_->endFrame)	{
			currentFrame_ = currentAnimData_->startFrame;
			isLoop = true;
		}
	}
	else {
		if (currentFrame_ > currentAnimData_->endFrame)	{
			currentFrame_ = currentAnimData_->endFrame;
		}
	}

	// ループした瞬間はdeltaを計算しない
	if (isLoop)
		return;

	XMFLOAT3 prevPos =
		Model::GetBonePosition(
			currentAnimData_->animPath,
			"mixamorig:Hips",
			prevFrame);

	XMFLOAT3 currPos =
		Model::GetBonePosition(
			currentAnimData_->animPath,
			"mixamorig:Hips",
			(int)currentFrame_);

	XMFLOAT3 delta;

	delta.x = currPos.x - prevPos.x;
	delta.y = 0.0f;
	delta.z = currPos.z - prevPos.z;

	// Root Motion
	transform_.position_.x += delta.x;
	transform_.position_.z += delta.z;

	// 1ループの移動距離を計測
	float dist = sqrtf(
		delta.x * delta.x +
		delta.z * delta.z);

	loopDistance += dist;

	
}

bool Player::SetState(AnimationState state)
{
	auto it = animData_.find(state);
	if (it == animData_.end()) {
		return false;
	}

	currentState_ = state;
	currentAnimData_ = &it->second;
	currentFrame_ = (float)currentAnimData_->startFrame;

	return true;
}

int Player::GetFrame() const
{
	return (int)currentFrame_;
}

AnimationState Player::StringToState(const string& (str))
{
	if (str == "IDLE") 
		return AnimationState::IDLE;
	
	if (str == "WALK") 
		return AnimationState::WALK;


	return AnimationState::STATE_MAX;
}

float Player::GetWalkAnimSpeed()
{
	const float loopDistance = 2.890786f;

	float loopPerSec = param_.MOVE_SPEED / loopDistance;

	float frameCount =
		float(currentAnimData_->endFrame -
			currentAnimData_->startFrame);

	float framePerSec = loopPerSec * frameCount;

	return framePerSec;
}

void Player::Draw()
{
	Model::SetFrame(hModel_, GetFrame());
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Player::Release()
{
	KillMe();
}




