#pragma once

#include "Blueprint/UserWidget.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBDivisionBackpackHud.h"
#include "BBBCombatHudWidget.generated.h"

class ABBBCharacter;
class ABBBPlayerController;
class APawn;
class UBBBCombatHudIndicator;
class UTextBlock;
class UWidget;

/** 驱动本地玩家全境封锁 2 风格背包侧边战斗 HUD 与中央瞄准层 */
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
    void InvalidateItems()
    {
        bItemsDirty = true;
    }

    /** @return 背包侧边悬浮战术集群控件 */
    const UBBBDivisionBackpackHud *GetBackpackHud() const
    {
        return BackpackHud;
    }

private:
    struct FTeammateOverheadCache
    {
        double LastLosCheckAt = -10.0;
        FVector CachedOverheadWorld = FVector::ZeroVector;
        float VisibleAlpha = 0.0f;
        float SmoothedArmor = 1.0f;
        float SmoothedHealth = 1.0f;
        bool bHasLineOfSight = false;
        bool bInitialized = false;
    };

    void UpdateVitals(const ABBBPlayerController &Controller, float DeltaTime, double Now);
    void UpdateItems(const ABBBPlayerController &Controller);
    void UpdateAnchorMotion(const ABBBPlayerController &Controller, const FGeometry &Geometry, float DeltaTime);
    void UpdateTeammates(const ABBBPlayerController &Controller, const FGeometry &Geometry, float DeltaTime, double Now);
    void SetCount(UTextBlock *Label, int32 Number);

    /** 背包侧边悬浮战术主集群 */
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBDivisionBackpackHud> BackpackHud;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> Crosshair;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> AimPoint;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UBBBCombatHudIndicator> AmmoRail;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UWidget> Telemetry;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> FrameRateValue;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UWidget> PingGroup;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> PingValue;

    /** 背包投影锚点弹簧平滑时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, Category = "空间", meta = (DisplayName = "锚点平滑时长", ClampMin = "0.01", ClampMax = "0.3"))
    float AnchorSmoothSeconds = 0.065f;

    /** 队友头顶界面最大可见距离 单位厘米 */
    UPROPERTY(EditDefaultsOnly, Category = "空间", meta = (DisplayName = "队友头顶可视距离", ClampMin = "500.0", ClampMax = "15000.0"))
    float TeammateMaxDistance = 4500.0f;

    FBBBDivisionHudSnapshot CachedSnapshot;
    TMap<FName, int32> DisplayedNumbers;
    TMap<TWeakObjectPtr<ABBBCharacter>, FTeammateOverheadCache> TeammateCaches;
    TWeakObjectPtr<APawn> ObservedPawn;

    FVector2D SmoothedAnchor = FVector2D::ZeroVector;
    FVector2D AnchorVelocity = FVector2D::ZeroVector;
    float SmoothedDistanceScale = 1.0f;
    float PreviousArmor = 0.0f;
    float PreviousMaximumArmor = 0.0f;
    float ArmorDamageAge = 1.0f;
    float ArmorDamageTrail = 1.0f;
    float PreviousHealth = 0.0f;
    float PreviousMaximum = 0.0f;
    float DamageAge = 1.0f;
    float DamageTrail = 1.0f;
    float StaminaFullSeconds = 2.0f;
    double SampleStartedAt = 0.0;
    int32 SampleFrames = 0;
    uint8 PreviousPhase = MAX_uint8;
    bool bItemsDirty = true;
    bool bLowHealth = false;
    bool bAnchorInitialized = false;
};
