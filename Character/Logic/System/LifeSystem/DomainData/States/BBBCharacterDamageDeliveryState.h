#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterDamageDeliveryState.generated.h"

class APawn;

/** 本帧已发生且需要投送到控制者的命中事实 */
USTRUCT()
struct FBBBCharacterDamageDeliveryState final
{
    GENERATED_BODY()

    /** 本地事实批次编号 */
    uint64 Serial = 0;

    /** 本帧伤害数值 */
    TArray<float> Damages;

    /** 命中骨骼 */
    TArray<FName> Bones;

    /** 命中世界位置 */
    TArray<FVector> Positions;

    /** 受力世界方向 */
    TArray<FVector> Directions;

    /** 命中来源角色 */
    TArray<TWeakObjectPtr<APawn>> Sources;
};
