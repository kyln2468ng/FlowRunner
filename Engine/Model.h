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
		int frame_;

		/*
		//アニメーションのフレーム
		float nowFrame, animSpeed;
		int startFrame, endFrame;


		//初期化
		ModelData() : pfbx_(nullptr), nowFrame(0), startFrame(0), endFrame(0), animSpeed(0)
		{
		}

		void SetAnimFrame(int start, int end, float speed)
		{
			nowFrame = (float)start;
			startFrame = start;
			endFrame = end;
			animSpeed = speed;
		}
		*/
	};
	int Load(std::string fileName);
	void SetTransform(int hModel, Transform transform);
	void Draw(int hModel); //後で聞く。一旦これで
	void Release();

	void SetFrame(int hModel, int frame);

	Fbx* GetFbx(int hModel);
	
	XMFLOAT3 GetBonePosition(int hModel, std::string boneName, int frame);

	void RayCast(int hModel,RayCastData& data);
	bool RayCastAll(int hModel, RayCastData& data,int& outModel);

}