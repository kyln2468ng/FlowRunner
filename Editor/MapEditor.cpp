#include "MapEditor.h"
#include "../Engine/Input.h"
#include "../Source/Stage.h"
#include "../Engine/Camera.h"

void MapEditor::Initialize(Stage* stage)
{
	stage_ = stage;
	previewPos_ = {};
}

void MapEditor::Updata()
{
	/*
	1 レイでhitpos取得
	2 法線方向にずらす
	3 グリッド化
	4 配置
	*/

	UpdateCamera();

	UpdateRay();

	UpdatePreview();

	PlaceBlock();
	
}

void MapEditor::Draw()
{
	//画面サイズ800*600
	int w = 400;
	int h = 300;

}

void MapEditor::UpdateCamera()
{
}

void MapEditor::UpdateRay()
{
	XMFLOAT3 camPos = Camera::GetPosition();
	XMFLOAT3 camTarget = Camera::GetTarget();



	XMVECTOR start = XMLoadFloat3(&camPos);
	XMVECTOR target = XMLoadFloat3(&camTarget);

	XMVECTOR dir = XMVector3Normalize(target - start);



	XMStoreFloat4(&ray_.start, start);
	XMStoreFloat4(&ray_.dir, dir);

	ray_.maxDist = 100.0f;

	stage_->hitObject(ray_);
}

void MapEditor::UpdatePreview()
{
	if (ray_.isHit)
	{
		XMVECTOR hitPos = XMLoadFloat3(&ray_.hitPos);
		XMVECTOR normal = XMLoadFloat3(&ray_.hitNormal);

		XMVECTOR placePos = hitPos + normal;

		XMFLOAT3 pos;
		XMStoreFloat3(&pos, placePos);

		pos.x = roundf(pos.x);
		pos.y = roundf(pos.y);
		pos.z = roundf(pos.z);

		PlaceBlock();

		previewPos_ = pos;
	}
}

void MapEditor::PlaceBlock()
{
	if (Input::IsKeyDown(DIK_P)) {
		stage_->CreateBlock(previewPos_);
	}
}
