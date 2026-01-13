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

struct RayCastData
{
	XMFLOAT4 start;	//Rayの始点
	XMFLOAT4 dir;	//Rayの方向（正規化してあること）
	bool isHit;		//当たったかどうか
	float dist;		//始点からの距離

	XMFLOAT3 hitPos;   //レイが当たった場所
	XMFLOAT3 normal;   //当たった面の法線
};

class Fbx
{
public:
	Fbx();
	HRESULT Load(std::string fileName);
	void Draw(Transform& transform);
	void Release();

	void InitVertex(FbxMesh* mesh);
	void InitIndex(FbxMesh* mesh);
	void InitConstantBuffer();
	void InitMaterial(FbxNode* pNode);

	void RayCast(RayCastData& rayData);
private:
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4 diffuse;
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matNormal;
		XMFLOAT4	diffuse;
		BOOL		materialFlag; // マテリアルがあるかないか
	};

	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;
	};

	int vertexCount_;	//頂点数
	int polygonCount_;	//ポリゴン数
	int materialCount_;	//マテリアルの個数

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;

	std::vector<MATERIAL> pMaterialList_;
	std::vector<int> indexCount_; // マテリアルごとのインデックス数

	std::vector<VERTEX>                 pVertices_;        // 頂点データ全部
	std::vector<std::vector<int>>       ppIndex_;   // マテリアルごとのインデックスデータ[material][index]

	XMFLOAT3 hitPos_;
	XMFLOAT3 nolmal_;

	//auto& arr = ppIndex_[1];
	//arr[0]～arr[index - 1];
};

