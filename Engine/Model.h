#pragma once
#include <string>
#include <vector>
#include "Fbx.h"

static Fbx* GetFbx(int hModel);

namespace Model
{
	struct ModelData
	{
		Fbx* pfbx_;
		Transform transform_;
		std::string filename_;

	};
	int Load(std::string fileName);
	void SetTransform(int hModel, Transform transform);
	void Draw(int hModel);
	void Release();

	Fbx* GetFbx(int hModel);

	void RayCast(int hModel,RayCastData& data);
	bool RayCastAll(int hModel, RayCastData& data,int& outModel);

}