#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/Model.h"

class StageObject : public GameObject {
public:
	StageObject(GameObject* parent);
	StageObject(const std::string& filename,const XMVECTOR& position,const XMVECTOR& rotation, const XMVECTOR& scale);
	~StageObject();
	void Initialize() override;
	//void AddObject();
	bool CollideLine(RayCastData& data);
private:
	std::string filename_;
};