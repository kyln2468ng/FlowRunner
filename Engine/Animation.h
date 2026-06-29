#pragma once
#include <unordered_map>
#include <string>


struct AnimationClip
{
	int startFrame;
	int endFrame;

	float speed;
	bool loop;
};

class Animation
{
public:
	Animation();
	~Animation();

	void Initialize();
	void Load();
	bool SetState(const std::string& state); // 状態切り替え
	void Update();                       // フレーム更新

	int GetFrame() const;                // 現在フレーム取得
private:

	//void SetAnimFram(int start,int end,float speed); //アニメーションのパラメータのセッター作ったが将来的にcsvから読もうと考えているため一旦保留

	std::unordered_map<std::string, AnimationClip> clips_;

	AnimationClip* currentClip_;

	float nowFrame_;
};