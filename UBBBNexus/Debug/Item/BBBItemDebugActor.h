#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBItemDebugActor.generated.h"

class ABBBCharacter;
class UBBBItemDefinition;

/** 通过角色输入入口一次性向背包添加指定物品的调试演员 */
UCLASS(Blueprintable, meta = (DisplayName = "物品调试器"))
class ABBB_EVAC_API ABBBItemDebugActor : public AActor
{
    GENERATED_BODY()

public:
    /** @return 构造等待本地角色就绪的调试演员 */
    ABBBItemDebugActor();

    //~ Begin AActor Interface
    /** @return 校验物品列表并开始等待 */
    virtual void BeginPlay() override;

    /**
     * 角色就绪后只提交一次入包请求
     * @param DeltaSeconds	本帧间隔秒数
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;
    //~ End AActor Interface

protected:
    /** 指定武器和穿戴品等物品 必须登记在目标角色的统一物品目录中 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|物品调试", meta = (DisplayName = "物品列表"))
    TArray<TObjectPtr<UBBBItemDefinition>> ItemDefinitions;

    /** 显式目标角色 留空时使用本地玩家角色 */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BBB|物品调试", meta = (DisplayName = "目标角色"))
    TObjectPtr<ABBBCharacter> TargetCharacter;

    /** 未指定目标时查找的本地玩家索引 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|物品调试", meta = (ClampMin = "0", DisplayName = "玩家索引"))
    int32 PlayerIndex = 0;

    /** 等待角色及其动画初始化的最长秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|物品调试", meta = (ClampMin = "0.1", DisplayName = "等待超时时间"))
    float WaitTimeout = 10.0f;
};
