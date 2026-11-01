#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterPhysicalImpact.h"
#include "BBBCharacterHitState.generated.h"

/** 最近有效伤害产生的骨骼受力事实 */
USTRUCT()
struct FBBBCharacterHitState final
{
    GENERATED_BODY()

    /** 当前生命周期内的命中编号 */
    uint64 Serial = 0;

    /** 命中骨骼 */
    FName Bone = NAME_None;

    /** 命中世界位置 */
    FVector Position = FVector::ZeroVector;

    /** 受力世界方向 */
    FVector Direction = FVector::ForwardVector;

    /** 事实成立的世界时间 */
    double Time = 0.0;
    /** 最新已成立的全身冲击版本 不被后续普通命中覆盖 */
    uint64 ImpactSerial = 0;
    /** 最新有效的全身物理冲击 */
    FBBBCharacterPhysicalImpact Impact;
};
