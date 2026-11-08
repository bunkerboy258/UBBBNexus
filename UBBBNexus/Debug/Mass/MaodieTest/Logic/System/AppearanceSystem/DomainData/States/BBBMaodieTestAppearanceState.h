#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "BBBMaodieTestAppearanceState.generated.h"
class UPointLightComponent;
class USkeletalMeshComponent;

/** 耄耋本机灯光与可恢复的模型显示状态 */
USTRUCT()
struct FBBBMaodieTestAppearanceState final
{
    GENERATED_BODY()

    /** 实例独占的正面柔光 */
    UPROPERTY(Transient)
    TObjectPtr<UPointLightComponent> Fill;

    /** 实例独占的侧后轮廓光 */
    UPROPERTY(Transient)
    TObjectPtr<UPointLightComponent> Rim;

    /** 已记录原始显示属性的身体 */
    UPROPERTY(Transient)
    TWeakObjectPtr<USkeletalMeshComponent> Mesh;

    /** 身体原有照明通道 */
    FLightingChannels OriginalChannels;

    /** 身体原有自定义深度开关 */
    bool bOriginalCustomDepth = false;

    /** 身体原有模板值 */
    int32 OriginalStencil = 0;

    /** 身体原有模板写入掩码 */
    ERendererStencilMask OriginalWriteMask = ERendererStencilMask::ERSM_Default;

    /** 平滑后的观察方向 避免灯位跳变 */
    FRotator ViewRotation = FRotator::ZeroRotator;
};
