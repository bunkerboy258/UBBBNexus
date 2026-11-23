#pragma once

#include "Blueprint/UserWidget.h"
#include "BBBCombatHudWidget.generated.h"

class ABBBPlayerController;
class APawn;
class UBBBCombatHudIndicator;
class UImage;
class UMaterialInterface;
class UProgressBar;
class UTextBlock;
class UTexture2D;

/** 将所属玩家的只读展示数据更新到 UMG 控件 */
UCLASS(Abstract, Blueprintable)
class ABBB_EVAC_API UBBBCombatHudWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UBBBCombatHudWidget(const FObjectInitializer &ObjectInitializer);

    /**
     * 更新当前本地玩家的战斗 HUD
     * @param Controller	所属玩家控制器
     * @param Geometry	控件所在的玩家画面几何
     * @param DeltaTime	本帧时间
     * @return 无
     */
    void UpdatePresentation(const ABBBPlayerController *Controller, const FGeometry &Geometry, float DeltaTime);
    /** @return 无 标记物品展示将在下一次可见更新时读取 */
    void InvalidateItems() { bItemsDirty = true; }

private:
    void UpdateVitals(const ABBBPlayerController &Controller, float DeltaTime, double Now);
    void UpdateItems(const ABBBPlayerController &Controller);
    void SetIcon(UImage *Image, UTexture2D *Texture);
    void SetCount(UTextBlock *Label, int32 Number, bool bPad = false, bool bDenominator = false);
    void SetItemRow(const TCHAR *Prefix, int32 Row, UTexture2D *Texture, int32 Number, bool bPad = false);

    UPROPERTY(EditDefaultsOnly, Category = "Appearance", meta = (DisplayName = "物品剪影材质"))
    TObjectPtr<UMaterialInterface> ItemIconMaterial;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UProgressBar> HealthBar;
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UProgressBar> DamageTrailBar;
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UProgressBar> StaminaBar;
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> Crosshair;
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> AimPoint;
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> AmmoRail;
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> Reload;

    UPROPERTY(Transient)
    TMap<TObjectPtr<UImage>, TObjectPtr<UTexture2D>> IconTextures;

    TMap<FName, int32> DisplayedNumbers;
    bool bItemsDirty = true;
    TWeakObjectPtr<APawn> ObservedPawn;
    float PreviousHealth = 0.0f;
    float PreviousMaximum = 0.0f;
    uint8 PreviousPhase = MAX_uint8;
    bool bLowHealth = false;
    float DamageAge = 1.0f;
    float DamageTrail = 0.0f;
    double SampleStartedAt = 0.0;
    int32 SampleFrames = 0;
};
