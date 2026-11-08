
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BBBPlayerState.generated.h"


//将下方类型注册为受虚幻对象系统管理的类
UCLASS()
//封装ABBB玩家状态的数据与行为
class ABBB_EVAC_API ABBBPlayerState : public APlayerState
{
    GENERATED_BODY()
public:

    /**
     * 创建并配置玩家状态 设置网络更新频率
     */
    ABBBPlayerState();

    /**
     * 进入游戏时建立运行依赖
     */
    virtual void BeginPlay() override;
};
