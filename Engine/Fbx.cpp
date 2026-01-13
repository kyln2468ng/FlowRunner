#include "Fbx.h"
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
			return true;
		}
		return false;
	}
}

Fbx::Fbx()
    :pVertexBuffer_(nullptr),
    pIndexBuffer_(nullptr),
    pConstantBuffer_(nullptr),
    vertexCount_(0),
    polygonCount_(0),
	materialCount_(0)
{
}

HRESULT Fbx::Load(std::string fileName)
{
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
	FbxManager* pFbxManager = FbxManager::Create();

	//インポーターを生成
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//シーンオブジェクトにFBXファイルの情報を流し込む
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	fbxImporter->Import(pFbxScene);
	fbxImporter->Destroy();

	//メッシュ情報を取得
	FbxNode* rootNode = pFbxScene->GetRootNode();
	FbxNode* pNode = rootNode->GetChild(0);
	FbxMesh* mesh = pNode->GetMesh();

	//各情報の個数を取得

	vertexCount_ = mesh->GetControlPointsCount();	//頂点の数
	polygonCount_ = mesh->GetPolygonCount();	//ポリゴンの数
	materialCount_ = pNode->GetMaterialCount();

	InitVertex(mesh);		//頂点バッファ準備
	InitIndex(mesh);		//インデックスバッファ準備
	InitConstantBuffer();	//コンスタントバッファ準備
	InitMaterial(pNode);

	fs::current_path(basePath);

	//マネージャ解放
	pFbxManager->Destroy();

	return S_OK;
}

void Fbx::Draw(Transform& transform)
{
	Direct3D::SetShader(SHADER_3D); // シェーダーの設定
	transform.Calculation();

	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = transform.GetNormalMatrix();


	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	for (int i = 0; i < materialCount_;i++)
	{
		if (pMaterialList_[i].pTexture)
		{
			cb.materialFlag = TRUE;
			cb.diffuse = XMFLOAT4(1, 1, 1, 1);
		}
		else
		{
			cb.materialFlag = FALSE;
			cb.diffuse = pMaterialList_[i].diffuse;
		}


		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

		Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		//コンスタントバッファ
		Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
		Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用


		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);
		}

		// 描画
		Direct3D::pContext->DrawIndexed(indexCount_[i], 0, 0);
	}
}

void Fbx::Release()
{
}

void Fbx::InitVertex(FbxMesh* mesh)
{
	//頂点情報を入れる配列
	//VERTEX* vertices = new VERTEX[vertexCount_];
	pVertices_.resize(vertexCount_);
	
	//全ポリゴン
	for (long poly = 0; poly < polygonCount_; poly++)
	{
		//3頂点分
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//調べる頂点の番号
			int index = mesh->GetPolygonVertex(poly, vertex);

			//頂点の位置
			FbxVector4 pos = mesh->GetControlPointAt(index);
			//vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);
			pVertices_[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//頂点のUV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			//vertices[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);
			pVertices_[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 1.0f);
			//頂点の法線
			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(poly, vertex, normal);
			//vertices[index].normal = XMVectorSet((float)normal[0], (float)normal[1], (float)normal[2], 0.0f);
			pVertices_[index].normal = XMVectorSet((float)normal[0], (float)normal[1], (float)normal[2], 0.0f);
		}
	}

	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexCount_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	//data_vertex.pSysMem = vertices;
	data_vertex.pSysMem = pVertices_.data();

	HRESULT hr;
	hr = Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"頂点バッファの作成に失敗しました",L"エラー", MB_OK);
	}
}

void Fbx::InitIndex(FbxMesh* mesh)
{
	pIndexBuffer_ = new ID3D11Buffer * [materialCount_];

	//int* index = new int[polygonCount_ * 3];
	ppIndex_.resize(materialCount_);
	indexCount_ = std::vector<int>(materialCount_);

	for (int i = 0; i < materialCount_; i++)
	{
		//int count = 0;
		auto& indeces = ppIndex_[i];
		
		//全ポリゴン
		for (long poly = 0; poly < polygonCount_; poly++)
		{
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly);

			if (mtlId == i)
			{
				//3頂点分
				for (long vertex = 0; vertex < 3; vertex++)
				{
					//index[count] = mesh->GetPolygonVertex(poly, vertex);
					indeces.push_back(mesh->GetPolygonVertex(poly, vertex));
					//count++;
				}
			}
		}

		//indexCount_[i] = count;
		indexCount_[i] = (int)indeces.size();

		// インデックスバッファを生成する
		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		//bd.ByteWidth = sizeof(int) * polygonCount_ * 3;
		bd.ByteWidth = sizeof(int) * indexCount_[i];
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		//InitData.pSysMem = index;
		InitData.pSysMem = indeces.data();
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		HRESULT hr;
		hr = Direct3D::pDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_[i]);
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"インデックスバッファの作成に失敗しました", L"エラー", MB_OK);
		}
	}
}

void Fbx::InitConstantBuffer()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;
	hr = Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"コンスタントバッファの作成に失敗しました", L"エラー", MB_OK);
	}
}

void Fbx::InitMaterial(FbxNode* pNode)
{
	//materialCount_の数だけ配列を準備
	pMaterialList_.resize(materialCount_);
	for (int i = 0; i < materialCount_; i++)
	{
		//i番目のマテリアル情報を取得
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		//テクスチャ情報
		FbxProperty IProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		//テクスチャの数数
		int fileTextureCount = IProperty.GetSrcObjectCount<FbxFileTexture>();

		//テクスチャあり
		if (fileTextureCount > 0)
		{
			FbxFileTexture* textureInfo = IProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();

			fs::path tPath(textureFilePath);
			if (fs::is_regular_file(tPath))
			{
				//ここでテクスチャの読み込み
				pMaterialList_[i].pTexture = new Texture;
				pMaterialList_[i].pTexture->Load(textureFilePath);
				//int a = 0;
				//a++;
			}
			else
			{
				//テクスチャファイルが無いときの処理(エラー）

			}

		}
		//テクスチャ無し
		else
		{
			//テスクチャないときの処理
			pMaterialList_[i].pTexture = nullptr;
			FbxDouble3 color = ((FbxSurfaceLambert*)pMaterial)->Diffuse.Get();
			pMaterialList_[i].diffuse = { (float)color[0],(float)color[1],(float)color[2],1.0f };
			//pMaterialList_[i].diffuse = XMDLOAT4{ (float)color[0],(float)color[1],(float)color[2],1.0f };
		}

	}
}

void Fbx::RayCast(RayCastData& rayData)
{
	//XMVECTOR start = XMLoadFloat4(&rayData.start);
	//XMVECTOR dir = XMLoadFloat4(&rayData.dir);
	//XMVECTOR dirN = XMVector4Normalize(dir); // dirの単位ベクトル

	//XMFLOAT3 start = { rayData.start.x,rayData.start.y,rayData.start.z };
	//XMFLOAT3 dir = { rayData.dir.x,rayData.dir.y, rayData.dir.z };
	//XMVECTOR vDir = XMLoadFloat3(&dir);
	//XMVECTOR vDirN = XMVector3Normalize(vDir);
	//XMStoreFloat3(&dir, vDirN);

	//XMFLOAT3 F0, F1, F2;

	for (int material = 0;material < materialCount_;material++)
	{
		auto& indices = ppIndex_[material];

		//全ポリゴンに対して
		for (int i = 0;i < (int)indices.size();i += 3)
		{
			VERTEX& V0 = pVertices_[ indices[i + 0] ];
			VERTEX& V1 = pVertices_[ indices[i + 1] ];
			VERTEX& V2 = pVertices_[ indices[i + 2] ];

			rayData.isHit = Math::Intersect(
				XMLoadFloat4(&rayData.start),
				XMLoadFloat4(&rayData.dir),
				V0.position,
				V1.position,
				V2.position,
				rayData.dist
			);

			if (rayData.isHit)
			{
				return;
			}
		}
	}
	rayData.isHit = false;


	//rayData.isHit = InterSects();

	//using namespace DirectX;

	//XMVECTOR start = XMLoadFloat4(&rayData.start);
	//XMVECTOR dir = XMVector3Normalize(XMLoadFloat4(&rayData.dir));

	//for (int m = 0; m < materialCount_; ++m)
	//{
	//	const auto& indices = indicesPerMat_[m];

	//	for (size_t i = 0; i + 2 < indices.size(); i += 3)
	//	{
	//		const VERTEX& v0 = vertices_[indices[i + 0]];
	//		const VERTEX& v1 = vertices_[indices[i + 1]];
	//		const VERTEX& v2 = vertices_[indices[i + 2]];

	//		//if (TriangleTests::Intersects(
	//		//	start, dir,
	//		//	v0.position, v1.position, v2.position,
	//		//	rayData.dist))
	//		//{
	//		//	rayData.hit = true;
	//		//	return;
	//		//}
	//	}
	//}
	//rayData.hit = false;
}
