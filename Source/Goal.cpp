#include "Goal.h"

Goal::Goal(GameObject* parent)
	:StageObject(parent)
{
	isGoal_ = false;
}

Goal::~Goal()
{
}

void Goal::Initialize()
{
}

void Goal::Draw()
{
}

bool Goal::IsGoal()
{
	return isGoal_;
}
