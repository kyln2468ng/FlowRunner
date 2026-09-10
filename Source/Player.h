#pragma once
#include "../Engine/GameObject.h"
#include "PlayerParamConfig.h"
#include <unordered_map>

class ChildOden;
class Bullet;

struct WallHitData
{
	bool isHit = false;
	float dist = FLT_MAX;
	XMVECTOR normal = XMVectorZero();
	XMVECTOR hitPos = XMVectorZero();
};

struct AnimationData
{
	int animPath;
	int startFrame;
	int endFrame;

	float speed;
	bool loop;
};

enum class AnimationState
{
	IDLE, 
	WALK,
	STATE_MAX
};

enum PlayerState
{
	IDLE,
	WALK,
	JUMP,
	FALL,
	WALL,
	WALL_JUMP,
	STATE_MAX
};


class Player : public GameObject
{
public:
	Player(GameObject* parent);
	~Player();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	XMFLOAT3 GetPos() { return transform_.position_; }
	int GetHandle() { return hModel_; }

private:
	WallHitData DetectWall(const XMVECTOR& vPos, const XMVECTOR& move, const XMVECTOR& right);
	
	void WallCling(const WallHitData& wall); //壁の吸着
	void WallCollision(XMVECTOR& vPos, XMVECTOR& move, const WallHitData& wall);//壁判定。めり込み防止と押し戻し
	void WallMove(XMVECTOR& move, const WallHitData& wall);
	void WallJump(const WallHitData& wall);
	bool IsWallTop(const XMVECTOR& vPos, const XMVECTOR& forward, float& groundY);

	void UpdateIdle();
	void UpdateWalk();
	void UpdateJump();
	void UpdateFall();
	void UpdateWall();
	void UpdateWallJump();

	///////アニメーション関連の関数///////////
	void LoadAnimData(const std::string& filePath);		// アニメーションのロード。アニメーション情報を読み込む（パラメータ）
	void AddAnimation(AnimationState state ,const std::string& animPath);		// アニメーションのパスの登録するの関数
	void LoadAnimation();								// アニメーションを実際に登録する
	void UpdateAnimation();								// 現在のアニメーションのフレーム更新
	bool SetState(AnimationState state);			// アニメーションの状態切り替え
	int GetFrame() const;								// 現在のフレーム取得
	AnimationState StringToState(const std::string& (str));
	float GetWalkAnimSpeed();

	/////////////////////////////////////////
	int hModel_;
	bool onGround_;
	bool justClimb_;
	PlayerParamConfig param_;
	XMFLOAT3 velocity_;
	bool isWall_;
	XMFLOAT3 wallNormal_;

	PlayerState state_;

	/////アニメーション関連の変数////
	std::unordered_map<AnimationState, AnimationData> animData_;	// アニメーション情報の配列（キー：状態名）
	AnimationData* currentAnimData_;							// 現在再生中のアニメーション情報
	float currentFrame_;										// 現在再生中のフレーム
	AnimationState currentState_;

	float loopDistance;

	int count = 0;
};