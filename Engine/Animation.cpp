#include "Animation.h"
#include "../Engine/CsvReader.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Initialize()
{
	clips_ = {};
	currentClip_ = nullptr;
	nowFrame_ = 0;
}

void Animation::Load()
{
	clips_.clear();

	CsvReader csv("Assets/model/playerAnimParam.csv");
	for (int i = 0; i < csv.GetLines();i++) {
		AnimationClip clip;

		std::string state = csv.GetString(i, 0);

		clip.startFrame = csv.GetInt(i, 1);
		clip.endFrame = csv.GetInt(i, 2);
		clip.speed = csv.GetFloat(i, 3);
		clip.loop = csv.GetInt(i, 4) != 0;

		clips_[state] = clip;
	}

}

bool Animation::SetState(const std::string& state)
{
	auto it = clips_.find(state);

	if (it == clips_.end())
		return false;

	currentClip_ = &it->second;

	nowFrame_ = (float)currentClip_->startFrame;

	return true;
}

void Animation::Update()
{
	if (!currentClip_) 
		return;

	nowFrame_ += currentClip_->speed;

	if (currentClip_->loop)
	{
		if (nowFrame_ > currentClip_->endFrame)
			nowFrame_ = currentClip_->startFrame;
	}
	else
	{
		if (nowFrame_ > currentClip_->endFrame)
			nowFrame_ = currentClip_->endFrame;
	}
}

int Animation::GetFrame() const
{
	return (int)nowFrame_;
}

