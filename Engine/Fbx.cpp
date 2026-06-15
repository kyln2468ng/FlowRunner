#include "Fbx.h"
#include "FbxParts.h"
#include "Direct3D.h"
#include "Camera.h"
#include <filesystem>
#include <string>
#include <DirectXCollision.h>

namespace fs = std::filesystem;

namespace Math
{
	//行列式を解く関数

	float Det(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c);
	float Det(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c)
	{
		float diag =    (a.x * b.y * c.z) + //左上から右下
						(a.y * b.z * c.x) + //左上から右下
						(a.z * b.x * c.y);  //左上から右下
		
		float rdiag =   (a.z * b.y * c.x) + //右上から左下
						(a.x * b.z * c.y) + //右上から左下
						(a.y * b.x * c.z);  //右上から左下

		return diag - rdiag;
	}

	//Ray
	bool Intersect(XMFLOAT3 origin, XMFLOAT3 ray, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, float& data);
	bool Intersect(XMFLOAT3 origin, XMFLOAT3 ray, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, float& data)
	{
		// Ray の始点・方向、三角形の頂点を XMVECTOR に変換
		// （DirectXMath で計算するため）
		XMVECTOR vOrigin = XMLoadFloat3(&origin); // Ray の開始点 O
		XMVECTOR vRay = XMLoadFloat3(&ray);    // Ray の方向ベクトル
		XMVECTOR vV0 = XMLoadFloat3(&v0);     // 三角形の頂点 v0
		XMVECTOR vV1 = XMLoadFloat3(&v1);     // 三角形の頂点 v1
		XMVECTOR vV2 = XMLoadFloat3(&v2);     // 三角形の頂点 v2

		//--------------------------------------
		// 三角形の 2 本の辺ベクトルを作る
		//--------------------------------------
		// edge1 = v1 - v0  （三角形の一辺）
		// edge2 = v2 - v0  （三角形のもう一辺）
		XMVECTOR vEdge1 = vV1 - vV0;
		XMVECTOR vEdge2 = vV2 - vV0;

		// 行列式計算のため、XMFLOAT3 に戻す
		XMFLOAT3 edge1;
		XMFLOAT3 edge2;
		XMStoreFloat3(&edge1, vEdge1);
		XMStoreFloat3(&edge2, vEdge2);


		//--------------------------------------
		// d = origin - v0
		//--------------------------------------
		// 三角形の基準点 v0 から、Ray の開始点 origin へのベクトル
		// 連立方程式
		//   t*ray = (v0 - origin) + u*edge1 + v*edge2
		// を作るための準備
		XMFLOAT3 d;
		XMStoreFloat3(&d, vOrigin - vV0);


		//--------------------------------------
		// ray を反転（-ray）
		//--------------------------------------
		// 連立方程式を
		//   u*edge1 + v*edge2 + t*(-ray) = d
		// の形にそろえるため、ray に -1 を掛ける
		ray = {
			ray.x * -1.0f,
			ray.y * -1.0f,
			ray.z * -1.0f
		};


		//--------------------------------------
		// 連立方程式の分母（行列式）
		//--------------------------------------
		// denom = det(edge1, edge2, -ray)
		// → 3 本のベクトルが作る行列の行列式
		// → 0 なら、平面と Ray が平行で交点を持たない
		float denom = Det(edge1,edge2,ray);


		// 平行（解なし）の判定
		if (denom <= 0.0f)
		{
			// Ray が三角形の平面と交差しない
			return false;
		}


		//--------------------------------------
		// クラメルの公式で u, v, t を求める
		//--------------------------------------

		// u = det(d, edge2, -ray) / denom
		// → 交点が edge1 方向にどれだけ進んだか（重心座標 u）
		float u = Det(d,edge2,ray) / denom;

		// v = det(edge1, d, -ray) / denom
		// → 交点が edge2 方向にどれだけ進んだか（重心座標 v）
		float v = Det(edge1,d,ray) / denom;

		// t = det(edge1, edge2, d) / denom
		// → Ray の開始点から交点までの距離
		float t = Det(edge1,edge2,d) / denom;


		//--------------------------------------
		// 三角形内部 ＋ Ray の向き 判定
		//--------------------------------------
		// t >= 0  : Ray の前方向に交点がある
		// u >= 0  : v0 → v1 方向に外れていない
		// v >= 0  : v0 → v2 方向に外れていない
		// u + v <= 1 : 三角形の内部に入っている
		if (t>=0 && u>= 0 && v >= 0&&u+v <= 1)
		{
			
			// Ray が三角形の内部にヒット
			data = t;
			return true;
		}

		// 条件を満たさない → 当たっていない
		return false;

	}
	
	bool Intersect(XMVECTOR origin, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float& data)
	{
		/*XMVECTOR vOrigin = XMLoadFloat4(&origin);
		XMVECTOR oDir = XMLoadFloat4(&dir);*/

		XMVECTOR vEdge1 = v1 - v0;
		XMVECTOR vEdge2 = v2 - v0;

		XMFLOAT3 edge1;
		XMFLOAT3 edge2;
		XMStoreFloat3(&edge1, vEdge1);
		XMStoreFloat3(&edge2, vEdge2);

		XMFLOAT3 d;
		XMStoreFloat3(&d, origin - v0);

		XMFLOAT3 fDir;
		XMStoreFloat3(&fDir, dir);
		/*fDir = {
			fDir.x * -1.0f,
			fDir.y * -1.0f,
			fDir.z * -1.0f,
		};*/

		float denom = Det(edge1, edge2, fDir);

		const float EPS = 1e-6f;
		if (fabs(denom) < EPS) return false;

		float u = Det(d, edge2, fDir) / denom;
		float v = Det(edge1, d, fDir) / denom;
		float t = Det(edge1, edge2, d) / denom;

		if (t >= 0 && u >= 0 && v >= 0 && u + v <= 1)
		{
			data = t;
			return true;
		}
		return false;
	}
}

Fbx::Fbx()
	:animSpeed_(0)
{
}

HRESULT Fbx::Load(std::string fileName)
{
	//FBXの読み込み
	using std::string;
	std::string subDir("Assets");
	fs::path currPath, basePath;
	currPath = fs::current_path();
	basePath = currPath;
	currPath = currPath / subDir;
	
	//fs::path subPath(currPath.string() + "\\" + subDir);
	assert(fs::exists(currPath));// subPathはあります、という確認
	fs::current_path(currPath);

	//マネージャを生成
	pFbxManager_ = fbxsdk::FbxManager::Create();

	//インポーターを生成
	FbxImporter* fbxImporter = fbxsdk::FbxImporter::Create(pFbxManager_, "imp");
	if (fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager_->GetIOSettings())) {
		//失敗
		return E_FAIL;
	}

	//シーンオブジェクトにFBXファイルの情報を流し込む
	pFbxScene_ = fbxsdk::FbxScene::Create(pFbxManager_, "fbxscene");
	fbxImporter->Import(pFbxScene_);
	fbxImporter->Destroy();

	//アニメーションのタイムモード取得
	_frameRate = pFbxScene_->GetGlobalSettings().GetTimeMode();

	// 現在のカレントディレクトリを保存
	fs::path oldPath = fs::current_path();

	// FBXのパス
	fs::path fbxPath(fileName);

	// FBXがあるフォルダへ移動
	fs::current_path(fbxPath.parent_path());



	//ルートノードを取得して
	FbxNode* rootNode = pFbxScene_->GetRootNode();

	//そいつの子供の数を調べて
	int childCount = rootNode->GetChildCount();

	//1個ずつチェック
	for (int i = 0; childCount > i; i++)
	{
		CheckNode(rootNode->GetChild(i), &parts_);
	}

	// 元に戻す
	fs::current_path(oldPath);

	return S_OK;
}

void Fbx::CheckNode(FbxNode* pNode, std::vector<FbxParts*>* pPartsList)
{
	//そのノードにはメッシュ情報が入っているか調べる
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//パーツ用意
		FbxParts* pParts = new FbxParts;
		pParts->Init(pNode);

		//パーツ情報を動的配列に追加
		pPartsList->push_back(pParts);
	}


	//子ノードのデータも調べる
	{
		//子供の数を調べる
		int childCount = pNode->GetChildCount();

		//チェック
		for (int i = 0; i < childCount; i++)
		{
			CheckNode(pNode->GetChild(i), pPartsList);
		}
	}
}

void Fbx::Draw(Transform& transform, int frame)
{	
	//パーツを1個ずつ描画
	for (int k = 0; k < parts_.size(); k++)
	{
		// その瞬間の自分の姿勢行列を得る
		FbxTime     time;
		time.SetTime(0, 0, 0, frame, 0, 0, _frameRate);

		//スキンアニメーション（ボーン有り）の描画
		if (parts_[k]->GetSkinInfo() != nullptr)
		{
			parts_[k]->DrawSkinAnime(transform, time);
		}

		//メッシュアニメーションの描画
		else
		{
			parts_[k]->DrawMeshAnime(transform, time, pFbxScene_);
		}
	}
}

void Fbx::Release()
{
	for (int i = 0; i < parts_.size(); i++)
	{
		delete parts_[i];
	}
	parts_.clear();

	pFbxScene_->Destroy();
	pFbxManager_->Destroy();
}

XMFLOAT3 Fbx::GetBonePosition(std::string boneName)
{
	XMFLOAT3 position = XMFLOAT3(0, 0, 0);
	for (int i = 0; i < parts_.size(); i++)
	{
		if (parts_[i]->GetBonePosition(boneName, &position))
			break;
	}

	return position;
}

void Fbx::RayCast(RayCastData& rayData)
{
	//すべてのパーツと判定
	for (int i = 0; i < parts_.size(); i++)
	{
		parts_[i]->RayCast(rayData);
	}
}


