#pragma once
#include "CoreMinimal.h"
#include "BBBWorldDropPresentationState.generated.h"
class UMaterialInstanceDynamic;
class UBBBItemDefinition;
/** 本地掉落物显示资源的独立生命周期 */
USTRUCT()
struct FBBBWorldDropPresentationState final
{
    GENERATED_BODY()
    /** 当前已解析的物品显示来源 */
    UPROPERTY()
    TObjectPtr<UBBBItemDefinition> RenderedDefinition = nullptr;
    /** 当前白色图标的动态材质 */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> IconMaterial = nullptr;
};
