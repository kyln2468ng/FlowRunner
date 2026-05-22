#include "Animation.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Initialize()
{
	currentTime_ = 0.0f;
	maxFrame_ = 0.0f;
	isLoop_ = true;
	isPlay_ = true;
}

void Animation::Update(float deltatime)
{
	if (isPlay_) {
		currentTime_ += deltatime;

		if (isLoop_) {
			while (currentTime_ >= maxFrame_)
			{
				currentTime_ -= maxFrame_;
			}
		}
		else {
			if (currentTime_ >= maxFrame_) {
				currentTime_ = maxFrame_;
				isPlay_ = false;
			}
		}
	}
}

void Animation::Play()
{
	isPlay_ = true;
}

void Animation::Stop()
{
	isPlay_ = false;
}

void Animation::SetMaxFrame(float maxFrame)
{
	maxFrame_ = maxFrame;
}

float Animation::GetTime() const
{
	return currentTime_;
}
