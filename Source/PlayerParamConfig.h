#pragma once

struct PlayerParamConfig
{
	float JUMP_HEIGHT;	// ジャンプの高さ
	float MOVE_SPEED;	// 移動速度
	float GRAVITY;		// 重力
};

const PlayerParamConfig& GetPlayerParamConfig();
