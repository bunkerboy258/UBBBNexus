#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Network/BBBNetworkConfig.h"
#include "Engine/DataAsset.h"
#include "BBBCharacterConfig.generated.h"

/** 角色全部可编辑的静态配置资产 */
UCLASS(BlueprintType)
//聚合角色全部可编辑运行配置
class ABBB_EVAC_API UBBBCharacterConfig final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterLocomotionConfig Locomotion;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBAimAnimationConfig AimAnimation;

    /** 角色动画事实识别配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterAnimationConfig Animation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterEquipmentConfig Equipment;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterNetworkConfig Network;
};
