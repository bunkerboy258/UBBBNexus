#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterMontageRequest.h"
#include "BBBCharacterMontageSlot.generated.h"

/** 固定槽位期望状态，多槽蒙太奇通过相同修订号共享一次播放 */
USTRUCT()
struct FBBBCharacterMontageSlot final
{
    GENERATED_BODY()

    /** 当前期望播放的蒙太奇请求 */
    UPROPERTY()
    FBBBCharacterMontageRequest Desired;

    /** 当前请求的播放修订号 */
    uint64 Revision = 0;
};
