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
	};
	int Load(std::string fileName);
	void SetTransform(int hModel, Transform transform);
	void Draw(int hModel);
	void Release();

	Fbx* GetFbx(int hModel);

	void SetAnimFrame(int hModel, int startFrame, int endFrame, float animSpeed);
	//現在のアニメーションのフレームを取得
	int GetAnimFrame(int hModel);
	
	XMFLOAT3 GetBonePosition(int hModel, std::string boneName);

	void RayCast(int hModel,RayCastData& data);
	bool RayCastAll(int hModel, RayCastData& data,int& outModel);

}