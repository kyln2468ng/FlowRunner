#pragma once
#include <DirectXMath.h>
#include "Quaternion.h"

using namespace DirectX;

//位置、向き、拡大率などを管理するクラス
class Transform
{
	XMMATRIX matTranslate_;	//移動行列
	XMMATRIX matRotate_;	//回転行列	
	XMMATRIX matScale_;	//拡大行列

public:
	XMFLOAT3 position_;			//位置
	math::Quaternion rotate_;	//向き
	XMFLOAT3 scale_;			//拡大率
	
	Transform* pParent_; //親のTransform

	//コンストラクタ
	Transform();

	//デストラクタ
	~Transform();

	//各行列の計算
	void Calculation();

	//ワールド行列を取得
	XMMATRIX GetWorldMatrix();
	XMMATRIX GetNormalMatrix();

	void SetVectorPosition(const XMVECTOR& vPos);
	void SetVectorRotation(const XMFLOAT3& euler);
	//void SetVectorRotation(const XMVECTOR& vRot);
	void SetVectorRotation(const math::Quaternion& quaternion);
	void SetVectroScale(const XMVECTOR& vScl);

};