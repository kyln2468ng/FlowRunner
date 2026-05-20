#include "MapEditor.h"
#include "../Engine/Input.h"
#include "../Source/Stage.h"
#include "../Engine/Camera.h"
#include <fstream>

void MapEditor::Initialize(Stage* stage)
{
	stage_ = stage;
	previewPos_ = {};

	//cameraPos_ = { 0, 10, -10 };

	yaw_ = 0.0f;
	pitch_ = 0.0f;
	mouse_ = XMVectorSet(400, 300, 0, 0);

	debugHandle_ = Model::Load("BoxDefault.fbx");
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

	//for (auto& d : debugRay_)
	//{
	//	Model::SetTransform(debugHandle_, d.transform);
	//	Model::Draw(debugHandle_);
	//}

	//Transform t;
	//t.position_ = previewPos_;
	//t.scale_ = { 1,1,1 };
	//t.Calculation();

	//Model::SetTransform(debugHandle_, t);

	//Model::Draw(debugHandle_);
}

void MapEditor::UpdateCamera()
{
	///マウス回転用
	XMVECTOR delta = Input::GetMouseDelta();

	float dx = XMVectorGetX(delta);
	float dy = XMVectorGetY(delta);

	//ここで条件入れればマウス回転のon/off可能
	yaw_ += dx * 0.005f;
	pitch_ -= dy * 0.005f;

	//キー入力回転用
	if (Input::IsKey(DIK_LEFT)) { 
		yaw_ -= 0.03f; 
	}
	if (Input::IsKey(DIK_RIGHT)) { 
		yaw_ += 0.03f; 
	} 
	if (Input::IsKey(DIK_UP)) {
		pitch_ += 0.02f; 
	}
	if (Input::IsKey(DIK_DOWN)) {
		pitch_ -= 0.02f; 
	}


	XMVECTOR forword = { cosf(pitch_) * sinf(yaw_), sinf(pitch_), cosf(pitch_) * cosf(yaw_), 0 };
	XMVECTOR right = { cosf(yaw_), 0, -sinf(yaw_), 0 };

	cameraPos_ = Camera::GetPosition();
	XMVECTOR pos = XMLoadFloat3(&cameraPos_);

	if (Input::IsKey(DIK_W)) {
		pos += forword * 0.2f;
	}
	if (Input::IsKey(DIK_S)) {
		pos -= forword * 0.2f;
	}
	if (Input::IsKey(DIK_A)) {
		pos -= right * 0.2f;
	}
	if (Input::IsKey(DIK_D)) {
		pos += right * 0.2f;
	}
	if (Input::IsKey(DIK_Q)) {
		pos += XMVectorSet(0, 0.2f, 0, 0);
	}
	if (Input::IsKey(DIK_E)) {
		pos -= XMVectorSet(0, 0.2f, 0, 0);
	}

	DirectX::XMStoreFloat3(&cameraPos_, pos);

	XMVECTOR target = pos + forword;


	Camera::SetPosition(cameraPos_);

	XMFLOAT3 targetPos;
	DirectX::XMStoreFloat3(&targetPos, target);

	Camera::SetTarget(targetPos);

	mouse_ = Input::GetMousePosition();
	float mouseX = XMVectorGetX(mouse_);
	float mouseY = XMVectorGetY(mouse_);
	printf("%f %f\n", mouseX, mouseY);

	XMVECTOR nearPos = XMVectorSet(mouseX, mouseY, 0.0f, 1.0f);
	XMVECTOR farPos = XMVectorSet(mouseX, mouseY, 1.0f, 1.0f);

	XMVECTOR nearWorld = XMVector3Unproject(
		nearPos,
		0.0f, 0.0f,
		800, 600,
		0.0f, 1.0f,
		Camera::GetProjectionMatrix(),
		Camera::GetViewMatrix(),
		XMMatrixIdentity()
	);
	XMVECTOR farWorld = XMVector3Unproject(
		farPos,
		0.0f, 0.0f,
		800, 600,
		0.0f, 1.0f,
		Camera::GetProjectionMatrix(),
		Camera::GetViewMatrix(),
		XMMatrixIdentity()
	);

	XMVECTOR start = nearWorld;
	XMVECTOR dir = XMVector3Normalize(farWorld - nearWorld);
	XMStoreFloat4(&ray_.dir, dir);
	XMStoreFloat4(&ray_.start, start);
	
	
	///debugchck///
	
	//printf("%f %f\n", mouseX, mouseY);

	//char buf[128];
	////sprintf_s(buf, "dist: %.2f\n", drawDist);

	//sprintf_s(buf, "mouse: %.2f %.2f %.2f\n", mouseX,mouseY);

	//OutputDebugStringA(buf);

	//for (int i = 0; i < 100; i++)
	//{
	//	XMVECTOR p = start + dir * (float)i;

	//	XMFLOAT3 pos;
	//	XMStoreFloat3(&pos, p);

	//	Transform t;
	//	t.position_ = pos;
	//	t.scale_ = { 0.001f,0.001f,0.001f };
	//	t.Calculation();

	//	DebugCube d;
	//	d.transform = t;
	//	d.life = 0.5f;
	//	debugRay_.push_back(d);
	//}

	//for (auto it = debugRay_.begin(); it != debugRay_.end(); )
	//{
	//	it->life -= 0.016f;

	//	if (it->life <= 0)
	//	{
	//		it = debugRay_.erase(it);
	//	}
	//	else
	//	{
	//		++it;
	//	}
	//}

}

void MapEditor::UpdateRay()
{
	//XMFLOAT3 camPos = Camera::GetPosition();
	//XMFLOAT3 camTarget = Camera::GetTarget();



	//XMVECTOR start = XMLoadFloat3(&camPos);
	//XMVECTOR target = XMLoadFloat3(&camTarget);

	//XMVECTOR dir = XMVector3Normalize(target - start);



	//XMStoreFloat4(&ray_.start, start);
	//XMStoreFloat4(&ray_.dir, dir);

	ray_.maxDist = 100.0f;

	stage_->HitBlock(ray_,-1);
}

void MapEditor::UpdatePreview()
{
	if (ray_.isHit)
	{
		XMVECTOR hitPos = XMLoadFloat3(&ray_.hitPos);
		XMVECTOR normal = XMLoadFloat3(&ray_.hitNormal);

		XMVECTOR placePos = hitPos + normal * 0.5f;

		XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, placePos);

		pos.x = roundf(pos.x);
		pos.y = roundf(pos.y);
		pos.z = roundf(pos.z);

		previewPos_ = pos;
	}
}

void MapEditor::PlaceBlock()
{
	if (Input::IsMouseButtonDown(0)) {
		stage_->CreateBlock(previewPos_);
	}

	if (Input::IsMouseButtonDown(1)) {
		stage_->DeleteBlock(stage_->GetModelIndex());
	}


	//ブロックの切り替え
}

void MapEditor::Save()
{
	std::ofstream file("stage.json");
	auto& blocks = stage_->SetBlock();

	file << "{\n";
	file << "\"blocks\": [\n";

	//書き込み

	file << "]\n";
	file.close;
}
