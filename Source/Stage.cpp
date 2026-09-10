#include "Stage.h"
#include "Player.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../resource.h"
#include "../Editor/MapEditor.h"
#include "StageObject.h"
#include "Goal.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), hModel_(-1),param_(GetPlayerParamConfig())
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	player_ = (Player*)Instantiate<Player>(this);

	editor_ = new MapEditor();
	editor_->Initialize(this);

	hitModelIndex_ = -1;

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
	
	for (int j = 0; j < ZSIZE; j++) {
		for (int i = 0; i < XSIZE; i++) {

			Block block{};
			block.type = DEFAULT;

			int h = Model::Load("BoxDefault.fbx");
			assert(h >= 0);
			block.handle = h;

			Transform t;
			t.position_ = { (float)i, -5.0f, (float)j };
			t.scale_ = { 1,1,1 };
			t.Calculation();

			block.transform = t;

			Model::SetTransform(block.handle, block.transform);
			models_.push_back(block);
			Model::SetRayCastTarget(block.handle, true);
		}

		
	}

	Transform t;
	t.position_ = { 10.0f, -3.0, 10.0f };
	t.scale_ = { 2,10,2 };
	t.Calculation();
	Block block{};
	block.handle = hModelColl_;
	block.type = DEFAULT;
	block.transform = t;
	Model::SetTransform(hModelColl_, block.transform);
	models_.push_back(block);
	Model::SetRayCastTarget(block.handle, true);

	t.position_ = { 20.0f, -3.0, 10.0f };
	t.scale_ = { 2,10,2 };
	t.Calculation();
	Block b{};
	b.handle = Model::Load("BoxDefault.fbx");
	b.type = DEFAULT;
	b.transform = t;
	Model::SetTransform(hModelColl_, b.transform);
	models_.push_back(b);
	Model::SetRayCastTarget(b.handle, true);


	goal = (Goal*)Instantiate<Goal>(this); //一旦仮
	stageObjects_.push_back(goal);
	isGoal_ = false;

	deathHeight_ = -10.0;
	isDead_ = false;

	isEditor_ = false;
}

void Stage::Update()
{
	XMFLOAT3 playerPos = player_->GetPos();
	if (playerPos.y < deathHeight_) {
		isDead_ = true;
	}

	for (StageObject* obj : stageObjects_) {
		player_->OnCollision(obj);
		//player_->Collision(obj);
	}

	if (goal->IsGoal()) {
		isGoal_ = true;
	}
		
	if (Input::IsKeyDown(DIK_M)) {
		isEditor_ = !isEditor_;
	}

	if (isEditor_ == true) {

		editor_->Updata();
	}

}

void Stage::Draw()
{
	for (auto& b : models_) {
	
		if (b.isAlive) {
			Model::SetTransform(b.handle, b.transform);
			Model::Draw(b.handle);
		}
	}

	if (isEditor_ == true) {

		editor_->Draw();
	}
}

void Stage::Release()
{
	KillMe();
}

void Stage::OnCollision(GameObject* pTarget)
{
	
}

bool Stage::hitObject(RayCastData& data,int selfHandle)
{
	int hitModel;

	if (Model::RayCastAll(selfHandle, data, hitModel))
	{
		hitModelIndex_ = hitModel;
		return true;
	}

	return false;
}

bool Stage::IsGoal()
{
	return isGoal_;
}

bool Stage::isDead()
{
	return isDead_;
}

float Stage::PlayerMaxDist(const PlayerParamConfig& param)
{
	pJumpV0_ = sqrtf(2.0f * param.GRAVITY * param.JUMP_HEIGHT);
	airTime_ = (2.0f * pJumpV0_) / param.GRAVITY;

	return param.MOVE_SPEED * airTime_;
}

bool Stage::HitBlock(RayCastData& data, int selfHandle)
{
	data.isHit = false;
	data.dist = FLT_MAX;

	bool hit = false;
	float closest = FLT_MAX;

	for (int i = 0; i < models_.size(); i++)
	{
		// 死んでるブロック無視
		if (!models_[i].isAlive)
			continue;

		// 自分自身無視
		if (models_[i].handle == selfHandle)
			continue;

		RayCastData ray = data;

		Model::RayCast(models_[i].handle, ray);

		if (!ray.isHit)
			continue;

		if (ray.dist < closest)
		{
			closest = ray.dist;

			data = ray;

			// Block配列のindex
			hitModelIndex_ = i;

			hit = true;
		}
	}

	return hit;
}

void Stage::CreateBlock(XMFLOAT3 pos)
{
	Block block{};
	block.type = DEFAULT;

	int h = Model::Load("BoxDefault.fbx");
	block.handle = h;

	Transform t;
	t.position_ = pos;
	t.scale_ = { 1,1,1 };
	t.Calculation();

	block.transform = t;

	Model::SetTransform(block.handle, block.transform);
	models_.push_back(block);
}

void Stage::DeleteBlock(int index)
{
	if (index < 0 || index >= models_.size())
		return;

	models_[index].isAlive = false;
}

const std::vector<Block>& Stage::GetBlocks()
{
	return models_;
}

void Stage::AddBlock(const Block& src)
{
	Block block = src;

	std::vector<string> modelName
	{
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};

	block.handle = Model::Load(modelName[block.type]);

	block.transform.Calculation();

	Model::SetTransform(block.handle, block.transform);
	models_.push_back(block);
}

void Stage::ClearBlocks()
{
	models_.clear();
}