#pragma once
#include "StageObject.h"

class Goal : public StageObject{
public:
	Goal(GameObject* parent);
	~Goal();
	void Initialize();
	void Draw();

	bool IsGoal();

private:
	bool isGoal_;
};