#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"
#include "BBBCharacterMontageSlotState.generated.h"

/** 固定槽位期望状态 多槽蒙太奇共享播放修订号 */
USTRUCT()
struct FBBBCharacterMontageSlotState
{
    GENERATED_BODY()

    UPROPERTY()
    FName Slot = NAME_None;
    UPROPERTY()
    FBBBCharacterMontagePacket Desired;
    uint64 Revision = 0;
};
