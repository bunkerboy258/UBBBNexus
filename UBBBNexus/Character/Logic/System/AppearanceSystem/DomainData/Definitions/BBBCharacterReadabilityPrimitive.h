#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "BBBCharacterReadabilityPrimitive.generated.h"
class USkeletalMeshComponent;

/** 接管角色可读性显示前的机械属性 */
USTRUCT()
struct FBBBCharacterReadabilityPrimitive
{
    GENERATED_BODY()
    /** 被标记的角色部件 */
    UPROPERTY()
    TWeakObjectPtr<USkeletalMeshComponent> Component;
    /** 原始直接照明通道 */
    UPROPERTY()
    FLightingChannels Channels;
    /** 原始自定义深度开关 */
    UPROPERTY()
    bool bCustomDepth = false;
    /** 原始模板值 */
    UPROPERTY()
    int32 Stencil = 0;
    /** 原始模板写入掩码 */
    UPROPERTY()
    ERendererStencilMask WriteMask = ERendererStencilMask::ERSM_Default;
};
