#pragma once
#include "StageObject.h"

class Goal : public StageObject{
public:
	Goal(GameObject* parent);
	~Goal();
	void Initialize();
	void Draw();
	void Update();
	void Release();
	bool IsGoal();
	void OnCollision(GameObject* pTarget) override;

private:
	bool isGoal_;
	int gModel_;
};