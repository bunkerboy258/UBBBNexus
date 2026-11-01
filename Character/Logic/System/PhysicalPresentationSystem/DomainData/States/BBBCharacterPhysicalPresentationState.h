#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Definitions/BBBCharacterRagdollSnapshot.h"
#include "BBBCharacterPhysicalPresentationState.generated.h"

/** 局部受击与全身物理表现的独立应用记录 */
USTRUCT()
struct FBBBCharacterPhysicalPresentationState final
{
    GENERATED_BODY()
    /** 最新已经表现的命中 */
    uint64 HitSerial = 0;
    /** 全身布娃娃是否已经接管网格 */
    bool bRagdoll = false;
    /** 输入接受的全身物理请求 与已经执行的接管标记分离 */
    bool bRagdollRequested = false;
    /** 控制端采集或镜像输入还原的当前物理结果 */
    UPROPERTY()
    FBBBCharacterRagdollSnapshot RagdollSnapshot;
    /** 镜像已应用到物理身体的结果版本 */
    uint64 AppliedRagdollRevision = 0;
    /** 本机物理完成后采集的骨盆位置 */
    FVector PelvisLocation = FVector::ZeroVector;
};
