#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterReadabilityPrimitive.h"
#include "BBBCharacterReadabilityState.generated.h"
class UPointLightComponent;

/** 只在本机存在的角色可读性机械状态 */
USTRUCT()
struct FBBBCharacterReadabilityState
{
    GENERATED_BODY()
    /** 柔和补光 */
    UPROPERTY(Transient)
    TObjectPtr<UPointLightComponent> Fill;
    /** 肩背轮廓光 */
    UPROPERTY(Transient)
    TObjectPtr<UPointLightComponent> Rim;
    /** 生命周期结束时需要恢复的部件属性 */
    UPROPERTY(Transient)
    TArray<FBBBCharacterReadabilityPrimitive> Primitives;
    /** 平滑后的观察方向 */
    FRotator ViewRotation = FRotator::ZeroRotator;
};
