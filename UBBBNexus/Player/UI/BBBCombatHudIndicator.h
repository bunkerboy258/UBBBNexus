#pragma once

#include "Components/Widget.h"
#include "BBBCombatHudIndicator.generated.h"

/** 战斗界面可单独布局的线条图形 */
UENUM()
enum class EBBBCombatHudGlyph : uint8
{
    Crosshair,
    AimPoint,
    AmmoRail,
    Reload,
    FrameRate,
    Ping
};

class SBBBCombatHudIndicator;

/** 提供 Designer 可放置的战斗线条图形 */
UCLASS()
class ABBB_EVAC_API UBBBCombatHudIndicator final : public UWidget
{
    GENERATED_BODY()

public:
    /** @param Loaded	当前弹药 @param Capacity	弹药容量 @param bContinuous	连续填充 @return 无 */
    void SetAmmo(int32 Loaded, int32 Capacity, bool bContinuous);
    /** @param Angle	换弹进度图形的角度 @return 无 */
    void SetReloadAngle(float Angle);

protected:
    //~ Begin UWidget Interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void SynchronizeProperties() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UWidget Interface

private:
    /** 在控件边界内绘制的线条图形 */
    UPROPERTY(EditAnywhere, Category = "Appearance", meta = (DisplayName = "图形"))
    EBBBCombatHudGlyph Glyph = EBBBCombatHudGlyph::Crosshair;

    /** 线条颜色及不透明度 */
    UPROPERTY(EditAnywhere, Category = "Appearance", meta = (DisplayName = "线条颜色"))
    FLinearColor Tint = FLinearColor(1.0f, 1.0f, 1.0f, 0.96f);

    TSharedPtr<SBBBCombatHudIndicator> Indicator;
};
