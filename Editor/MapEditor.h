#pragma once
#include <DirectXMath.h>
#include "../Engine/Fbx.h"
#include <string>

class Stage;

struct DebugCube
{
	Transform transform;
	float life;
};

class MapEditor
{
public:
	void Initialize(Stage* stage);
	void Updata();
	void Draw();

	void UpdateCamera();
	void UpdateRay();
	void UpdatePreview();
	void PlaceBlock();

	std::string OpenFileDialog(bool isSave);
	void Save();
	void Load();

private:
	Stage* stage_;
	bool isEditor_;
	DirectX::XMFLOAT3 previewPos_;
	RayCastData ray_;
	bool isHit_;
	XMVECTOR mouse_;
	XMFLOAT3 cameraPos_;

	int debugHandle_;
	std::vector<DebugCube> debugRay_;

	float yaw_;
	float pitch_;
};