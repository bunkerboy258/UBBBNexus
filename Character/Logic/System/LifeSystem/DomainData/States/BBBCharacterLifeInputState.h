#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "BBBCharacterLifeInputState.generated.h"

class APawn;

/** 本次生命更新待消费的伤害与结果输入 */
USTRUCT()
struct FBBBCharacterLifeInputState final
{
    GENERATED_BODY()

    /** 同帧全部伤害数值 */
    TArray<float> Damages;

    /** 与伤害一一对应的骨骼 */
    TArray<FName> Bones;

    /** 与伤害一一对应的命中位置 */
    TArray<FVector> Positions;

    /** 与伤害一一对应的受力方向 */
    TArray<FVector> Directions;

    /** 与伤害一一对应的来源角色 */
    TArray<TWeakObjectPtr<APawn>> Sources;

    /** 是否存在待还原生命结果 */
    bool bHasResult = false;

    /** 待还原的生命阶段 */
    EBBBCharacterLifePhase ResultPhase = EBBBCharacterLifePhase::Alive;

    /** 待还原的生命数值 */
    float ResultHealth = 0.0f;

    /** 待还原结果标识 */
    uint64 ResultRevision = 0;

    /** 待还原的倒地轮次 */
    uint64 ResultDownedRevision = 0;
    /** 待还原的恢复碰撞姿态 */
    bool bResultRecoveryCrouched = false;

    /** 待还原命中编号 */
    uint64 ResultHitSerial = 0;

    /** 待还原命中骨骼 */
    FName ResultBone = NAME_None;

    /** 待还原命中位置 */
    FVector ResultPosition = FVector::ZeroVector;

    /** 待还原命中方向 */
    FVector ResultDirection = FVector::ForwardVector;
};
