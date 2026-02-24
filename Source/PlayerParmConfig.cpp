#include "PlayerParamConfig.h"
#include <Windows.h>
#include <string>

namespace
{
    std::string g_iniPath;
}

float ReadFloatIni(const char* paramKey)
{
    char buffer[64];
    GetPrivateProfileStringA("PlayerParam", paramKey, "0", buffer, sizeof(buffer), g_iniPath.c_str());
    return std::stof(buffer);
}

const PlayerParamConfig LoadPlayerParamConfig(const std::string& iniPath)
{
    PlayerParamConfig param;
    g_iniPath = iniPath;
    char buff[64];

    //GetPrivateProfileStringA("PlayerParam", "JUMP_HEIGHT", "0", buff, sizeof(buff), iniPath.c_str());
    //param.JUMP_HEIGHT = std::stof(buff);
        
    //param.MOVE_SPEED = std::stof(関数(keyを入れる))
    param.JUMP_HEIGHT = ReadFloatIni("JUMP_HEIGHT");
    param.MOVE_SPEED = ReadFloatIni("MOVE_SPEED");
    param.GRAVITY = ReadFloatIni("GRAVITY");

    return param;
}

const PlayerParamConfig& GetPlayerParamConfig()
{
    // TODO: return ステートメントをここに挿入します
    static PlayerParamConfig config = LoadPlayerParamConfig("./playerParam.ini");
    return config;
}

