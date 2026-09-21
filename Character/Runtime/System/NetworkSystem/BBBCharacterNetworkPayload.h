#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBCharacterNetworkPayload.generated.h"

/** 需要同步到模拟端的移动状态载荷 */
USTRUCT()
struct FBBBLocomotionNetworkPayload final
{
    GENERATED_BODY()

    /** 已经成立的角色步态 */
    UPROPERTY()
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;
};

/** 需要同步到模拟端的瞄准状态载荷 */
USTRUCT()
struct FBBBAimNetworkPayload final
{
    GENERATED_BODY()

    /** 是否保持瞄准 */
    UPROPERTY()
    bool bIsAiming = false;

    /** 使用网络定点压缩的世界空间瞄准目标 */
    UPROPERTY()
    FVector_NetQuantize10 AimTargetWorld = FVector::ZeroVector;
};
