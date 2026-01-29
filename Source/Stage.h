#pragma once
#include "../Engine/GameObject.h"
#include <vector>
#include "Windows.h"
#include "../Engine/Model.h"

class Enemy;
class Player;


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
	int type;
	Transform transform;

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

	bool hitObject(RayCastData& data);//オブジェクト（ブロック）と当たったかを返す

	bool CollideLine(RayCastData& data); //ステージオブジェクトとのレイキャスト取る
	//int  GetModelCount() const;
	//int  GetModel(int index) const;
	/*const std::vector<StageObject*>& GetStageObjects() const
	{
		return stageObjects_;
	}*/
	
private:
	std::vector<Enemy*> enemy_;
	int type_;
	int hModel_[MODEL_NUM];
	int hModelColl_;
	sData sTable[ZSIZE][XSIZE];
	int mode_; // 0：上げる　1：下げる　2：種類変更
	int select_; //ボックスの種類

	std::vector<Block> models_;
};