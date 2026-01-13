#pragma once
#include "Engine/GameObject.h"
#include <vector>
#include "Windows.h"

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
	BOOL localProck(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// 操作用のパネル用プロシージャ
	BOOL manuProck(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	std::vector<Enemy*> enemy_;
	int hModel_[MODEL_NUM];
	int hModelColl_;
	sData sTable[ZSIZE][XSIZE];
	int mode_; // 0：上げる　1：下げる　2：種類変更
	int select_; //ボックスの種類
};