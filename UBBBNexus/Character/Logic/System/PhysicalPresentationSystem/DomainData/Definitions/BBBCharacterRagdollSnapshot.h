#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Definitions/BBBCharacterPhysicalPhase.h"
#include "BBBCharacterRagdollSnapshot.generated.h"

/** 全身物理阶段与起身的当前结果 不保存逐骨骼历史 */
USTRUCT()
struct FBBBCharacterRagdollSnapshot final
{
    GENERATED_BODY()
    /** 当前实例内递增的物理结果版本 零表示尚未进入 */
    UPROPERTY()
    uint64 Revision = 0;
    /** 独立于连续采样的阶段版本 用于可靠投送 */
    UPROPERTY()
    uint64 Transition = 0;
    /** 每次进入物理时递增 拒绝旧起身通知 */
    UPROPERTY()
    uint64 Episode = 0;
    /** 当前身体控制阶段 */
    UPROPERTY()
    EBBBCharacterPhysicalPhase Phase = EBBBCharacterPhysicalPhase::Animated;
    /** 通过空间检查的起身胶囊变换 */
    UPROPERTY()
    FTransform RecoveryTransform;
    /** 目标空间只允许蹲伏 */
    UPROPERTY()
    bool bRecoveryCrouched = false;
    /** 控制端已选定的动作 镜像不重新选片 */
    UPROPERTY()
    TObjectPtr<class UAnimMontage> GetUpMontage = nullptr;
    /** 当前动作进度 仅用于远端恢复 */
    UPROPERTY()
    float GetUpPosition = 0.0f;
    /** 物理骨盆的世界位置 */
    UPROPERTY()
    FVector PelvisLocation = FVector::ZeroVector;
    /** 物理骨盆的世界线速度 */
    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;
};
