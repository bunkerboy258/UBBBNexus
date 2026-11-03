#pragma once
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "UObject/StrongObjectPtr.h"

class UTexture2D;
class APawn;
class AActor;

class UBBBPlayerItemView;

/** 只绘制玩家公开结果的极简战斗覆层 */
class SBBBCombatHud final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBCombatHud) : _View(nullptr) {}
        SLATE_ARGUMENT(UBBBPlayerItemView *, View)
    SLATE_END_ARGS()

    /** @param Arguments	所属玩家界面 @return 无 */
    void Construct(const FArguments &Arguments);
    virtual void Tick(const FGeometry &Geometry, double CurrentTime, float DeltaTime) override;
    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
        FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style, bool bParentEnabled) const override;

private:
    /** 所属界面不拥有玩法状态 */
    TWeakObjectPtr<UBBBPlayerItemView> View;
    TStrongObjectPtr<UTexture2D> Crosshair;
    TStrongObjectPtr<UTexture2D> Heart;
    TWeakObjectPtr<APawn> ObservedPawn;
    TWeakObjectPtr<AActor> ObservedEquipment;
    FGuid ConsumableId;
    int32 ConsumableSlot = INDEX_NONE;
    float PreviousHealth = 0.0f;
    float PreviousMaximum = 0.0f;
    uint8 PreviousPhase = 255;
    bool bLowHealth = false;
    bool bWasEmpty = false;
    float DamageAge = 1.0f;
    float DamageTrail = 0.0f;
    float EmptyAge = 1.0f;
    float NameAge = 2.0f;
};
