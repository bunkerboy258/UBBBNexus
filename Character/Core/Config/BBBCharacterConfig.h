#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Camera/BBBCameraConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBFacingConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBCharacterConfig.generated.h"

USTRUCT(BlueprintType)
//聚合角色全部可编辑运行配置
struct FBBBCharacterConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterInputConfig Input;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterLocomotionConfig Locomotion;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterFacingConfig Facing;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBAimConfig Aim;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBAimAnimationConfig AimAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterCameraConfig Camera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterEquipmentConfig Equipment;
};
