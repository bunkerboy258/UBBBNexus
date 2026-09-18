#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/Camera/BBBCharacterDiscreteCameraInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/Equipment/BBBCharacterDiscreteEquipmentInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/Fire/BBBCharacterDiscreteFireInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/Jump/BBBCharacterDiscreteJumpInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/Montage/BBBCharacterDiscreteMontageInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Discrete/Reload/BBBCharacterDiscreteReloadInput.h"
#include "BBBCharacterDiscreteInput.generated.h"

/** 外部动作和装备事实共用的离散输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterDiscreteEquipmentInput Equipment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterDiscreteFireInput Fire;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterDiscreteReloadInput Reload;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterDiscreteJumpInput Jump;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterDiscreteMontageInput Montage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterDiscreteCameraInput Camera;
};
