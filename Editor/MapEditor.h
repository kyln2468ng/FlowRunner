#pragma once
#include <DirectXMath.h>
#include "../Engine/Fbx.h"
class Stage;

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


private:
	Stage* stage_;
	bool isEditor_;
	DirectX::XMFLOAT3 previewPos_;
	RayCastData ray_;
	bool isHit_;

	XMFLOAT3 cameraPos_;

	float yaw_;
	float pitch_;
};