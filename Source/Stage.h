#pragma once
#include "../Engine/GameObject.h"
#include <vector>
#include "Windows.h"
#include "../Engine/Model.h"
#include "PlayerParamConfig.h"

class Enemy;
class Player;
class MapEditor;
class StageObject;
class Goal; //一旦仮、後で消す

namespace
{
	const int XSIZE{ 15 };
	const int ZSIZE{ 15 };
	const int MODEL_NUM{ 5 };
}


enum BLOCK_TYPE
{
	DEFAULT, BRICK, GRASS, SAND, WATER
};


struct sData
{
	BLOCK_TYPE type;//ブロックのタイプ
	int height;//ブロックを積む高さ
};

struct Block
{
	int handle = -1;
	BLOCK_TYPE type = BLOCK_TYPE::DEFAULT;

	Transform transform;
	bool isAlive = true;
};

class Stage : public GameObject
{
public:
	Stage(GameObject* parent);
	~Stage();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void OnCollision(GameObject* pTarget) override;

	void SetBlock(BLOCK_TYPE type, int x, int z) { GetT(x, z).type = type; }
	void SetBlockHeight(int x, int z, int height) { GetT(x, z).height = height; }
	sData& GetT(int x, int z) { return sTable[z][x]; }

	int GetModel() const { return hModelColl_; }

	bool IsEdhitorMode() const { return isEditor_; } //マップエディタのゲッター

	bool hitObject(RayCastData& data,int selfHandle);//オブジェクト（ブロック）と当たったかを返す


	//bool CollideLine(RayCastData& data); //ステージオブジェクトとのレイキャスト取る
	//int  GetModelCount() const;
	//int  GetModel(int index) const;
	/*const std::vector<StageObject*>& GetStageObjects() const
	{
		return stageObjects_;
	}*/
	
	float PlayerMaxDist(const PlayerParamConfig& param);

	//ステージに置くオブジェクトのモデルハンドルを取れるようにする
	//ステージに置くオブジェクトのモデルの選択
	//選択したモデルをセッターで保存→モデルのハンドル読む
	//当たったブロック取得
	//指定座標にブロックあるか　ブロックの重複禁止

	int GetModelIndex() const { return hitModelIndex_; } //レイキャスト（オール）で当たったモデルを返す
	bool HitBlock(RayCastData& data, int selfHandle);

	void CreateBlock(XMFLOAT3 pos);
	void DeleteBlock(int index);

	const std::vector<Block>& GetBlocks();
	void AddBlock(const Block& src);
	void ClearBlocks();

private:
	std::vector<Enemy*> enemy_;
	int type_;
	int hModel_[MODEL_NUM];
	int hModelColl_;
	sData sTable[ZSIZE][XSIZE];
	int mode_; // 0：上げる　1：下げる　2：種類変更
	int select_; //ボックスの種類

	std::vector<Block> models_; //ステージのオフジェクト配列

	PlayerParamConfig param_;
	float gap_; //ビルとビルとの幅
	float pJumpV0_;
	float airTime_;
	int hitModelIndex_;

	bool isEditor_;
	MapEditor* editor_;

	Player* player_;
	std::vector<StageObject*> stageObjects_;
	Goal* goal;
};