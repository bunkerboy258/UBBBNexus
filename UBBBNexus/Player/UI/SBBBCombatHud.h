#pragma once
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "UObject/StrongObjectPtr.h"

class APawn;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UTexture2D;

class UBBBPlayerItemView;

/** 只绘制玩家公开结果的极简战斗覆层 */
class SBBBCombatHud final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBCombatHud) : _View(nullptr), _ItemIconMaterial(nullptr) {}
        SLATE_ARGUMENT(UBBBPlayerItemView *, View)
        SLATE_ARGUMENT(UMaterialInterface *, ItemIconMaterial)
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
    /** 只保留当前可见图标的动态材质 */
    TWeakObjectPtr<UMaterialInterface> ItemIconMaterial;
    mutable TMap<TWeakObjectPtr<UTexture2D>, TStrongObjectPtr<UMaterialInstanceDynamic>> IconMaterials;
    TWeakObjectPtr<APawn> ObservedPawn;
    float PreviousHealth = 0.0f;
    float PreviousMaximum = 0.0f;
    uint8 PreviousPhase = 255;
    bool bLowHealth = false;
    float DamageAge = 1.0f;
    float DamageTrail = 0.0f;
    /** 本地性能读数 每半秒取样一次 */
    double SampleStartedAt = 0.0;
    int32 SampleFrames = 0;
    int32 FrameRate = 0;
    TOptional<int32> Ping;
};
