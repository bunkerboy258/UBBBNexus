#pragma once
#include "CoreMinimal.h"
class UBBBItemDefinition;
#include "BBBWorldDropInitialization.generated.h"
/** 一次生成所提供的物品与释放参数 */
USTRUCT()
struct FBBBWorldDropInitialization final
{
    GENERATED_BODY()
    /** 物品静态定义 */
    UPROPERTY()
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;
    /** 本份数量 */
    UPROPERTY()
    int32 Quantity = 1;
    /** 释放世界速度 */
    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;

};
