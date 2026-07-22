#pragma once
#include "../Engine/GameObject.h"

class StageObject : public GameObject {
public:
	StageObject(GameObject* parent);
	~StageObject();
	void Initialize() override;
	void Update() override;
	void Draw() override;
};