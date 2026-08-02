
#include "BBBWork/UBBBNexus/Player/BBBPlayerState.h"

//创建并配置ABBB玩家状态
ABBBPlayerState::ABBBPlayerState()
{
    //设置NetUpdateFrequency
    SetNetUpdateFrequency(100.0f);
    //设置MinNetUpdateFrequency
    SetMinNetUpdateFrequency(30.0f);
}

//进入游戏时建立运行依赖
void ABBBPlayerState::BeginPlay()
{
    //先执行父类的进入游戏时建立运行依赖
    Super::BeginPlay();
}
