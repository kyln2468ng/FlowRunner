#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include "Texture.h"
#include <vector>
//#include "Camera.h"


#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

class FbxParts;

struct RayCastData
{
	XMFLOAT4 start;	//Rayの始点
	XMFLOAT4 dir;	//Rayの方向（正規化してあること）
	bool isHit;		//当たったかどうか
	float dist;		//始点からの距離
	float maxDist; // レイの最大値

	XMFLOAT3 hitPos;   //レイが当たった場所
	XMFLOAT3 localHit;
	XMFLOAT3 hitNormal; // ヒットした面の法線


};

struct FBX_VERTEX
{
	XMVECTOR position;
	XMVECTOR uv;
	XMVECTOR normal;

	UINT boneIndex[4];
	float weight[4];
};

class Fbx
{
public:
	Fbx();

	//ロード
	virtual HRESULT Load(std::string fileName);

	//描画
	void Draw(Transform& transform, int frame);

	//解放
	void Release();

	//任意のボーンの位置を取得
	XMFLOAT3 GetBonePosition(std::string boneName);

	void RayCast(RayCastData& rayData);

private:
	friend class FbxParts; 	//FbxPartクラスをフレンドクラスにする
	std::vector<FbxParts*>	parts_; //モデルの各パーツ
	FbxManager* pFbxManager_; //FBXファイルを扱う機能の本体
	FbxScene* pFbxScene_; //FBXファイルのシーン（Mayaで作ったすべての物体）を扱う

	//後で分離//
	FbxTime::EMode _frameRate;	// アニメーションのフレームレート

	float animSpeed_; //アニメーション速度

	//アニメーションの最初と最後のフレーム
	int startFrame_;
	int endFrame_; 

	//ノードの中身を調べる
	void CheckNode(FbxNode* pNode, std::vector<FbxParts*>* pPartsList);
};

