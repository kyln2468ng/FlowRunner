#pragma once
#include <unordered_map>
#include <string>

class Animation
{
public:
	Animation();
	~Animation();

	void Initialize();
	void Load();
	void Update();

private:
	struct AnimationClip
	{
		int hModel;

		int startFrame;
		int endFrame;

		float speed;
		bool loop;
	};

	std::unordered_map<std::string, AnimationClip> clips_;

	AnimationClip* currentClip_;

	float currentFrame_;
};