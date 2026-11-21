#pragma once

#include "Components/WidgetComponent.h"
#include "BBBPlayerItemPanelComponent.generated.h"

/** 复用未发生绘制变化的背包空间面板纹理 */
UCLASS()
class UBBBPlayerItemPanelComponent final : public UWidgetComponent
{
    GENERATED_BODY()

public:
    explicit UBBBPlayerItemPanelComponent(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
    {
        bUseInvalidationInWorldSpace = true;
    }
};
