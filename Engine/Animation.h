#pragma once

class Animation
{
public:
	Animation();
	~Animation();

	void Initialize();
	void Update(float deltatime);
	void Play();
	void Stop();
	//void SetSpeed(float speed);				//モーションを何倍速で進めるかをセットする
	//void SetMotionData(MotionData* motion);	//引数を呼びたいモーションにしセットする
	void SetMaxFrame(float maxFrame);

	float GetTime() const;

private:
	float currentTime_;
	float maxFrame_;
	bool isLoop_;
	bool isPlay_;
};