#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentDebugActor.generated.h"

class ABBBCharacter;
class ABBBEquipment;

/** 通过角色输入入口一次性注入装备的独立调试演员 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBEquipmentDebugActor : public AActor
{
    GENERATED_BODY()

public:
    /** @return 构造仅在游戏中等待目标的调试演员 */
    ABBBEquipmentDebugActor();

    //~ Begin AActor Interface
    /** @return 校验调试配置并启动等待 */
    virtual void BeginPlay() override;

    /**
     * 等待角色就绪后仅提交一次装备状态输入
     * @param DeltaSeconds	本帧间隔秒数
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;
    //~ End AActor Interface

protected:
    /** 要注入的装备配置 必须已登记在目标角色现有装备目录中 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备调试", meta = (DisplayName = "装备类"))
    TArray<TSubclassOf<ABBBEquipment>> EquipmentClasses;

    /** 显式目标角色 留空时使用本地玩家角色 */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BBB|装备调试", meta = (DisplayName = "目标角色"))
    TObjectPtr<ABBBCharacter> TargetCharacter;

    /** 未指定目标时查找的本地玩家索引 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备调试", meta = (ClampMin = "0", DisplayName = "玩家索引"))
    int32 PlayerIndex = 0;

    /** 等待角色及其动画初始化的最长秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备调试", meta = (ClampMin = "0.1", DisplayName = "等待超时时间"))
    float WaitTimeout = 10.0f;
};
