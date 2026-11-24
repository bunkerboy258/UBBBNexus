#pragma once

#include "Components/Widget.h"
#include "BBBDivisionBackpackHud.generated.h"

class UFontFace;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UTexture2D;
class SBBBDivisionBackpackHud;

/** 背包侧边悬浮战斗集群的单个槽位快照 */
struct FBBBDivisionHudSlotSnapshot
{
    UTexture2D *Icon = nullptr;
    int32 Number = INDEX_NONE;
    bool bOccupied = false;
    bool bActive = false;

    bool operator==(const FBBBDivisionHudSlotSnapshot &Other) const
    {
        return Icon == Other.Icon
            && Number == Other.Number
            && bOccupied == Other.bOccupied
            && bActive == Other.bActive;
    }

    bool operator!=(const FBBBDivisionHudSlotSnapshot &Other) const
    {
        return !(*this == Other);
    }
};

/** 背包侧边悬浮战斗集群的每帧只读展示快照 */
struct FBBBDivisionHudSnapshot
{
    static constexpr int32 MaxWeaponSlots = 3;
    static constexpr int32 MaxSpareThrowables = 2;

    float HealthFraction = 1.0f;
    float DamageTrailFraction = 1.0f;
    float StaminaFraction = 1.0f;
    float StaminaOpacity = 0.28f;
    float LowHealthPulse = 0.0f;
    float AimBlend = 0.0f;
    float ReloadAngle = 0.0f;
    float DistanceScale = 1.0f;

    int32 LoadedAmmo = 0;
    int32 CapacityAmmo = 0;

    FVector2D AnchorPosition = FVector2D::ZeroVector;
    FVector2D ViewportSize = FVector2D(1920.0, 1080.0);

    UTexture2D *ActiveWeaponIcon = nullptr;
    FBBBDivisionHudSlotSnapshot WeaponSlots[MaxWeaponSlots];
    FBBBDivisionHudSlotSnapshot SelectedThrowable;
    FBBBDivisionHudSlotSnapshot SpareThrowables[MaxSpareThrowables];

    bool bHasAnchor = false;
    bool bHasAmmo = false;
    bool bAmmoContinuous = true;
    bool bReloading = false;
    bool bLowHealth = false;
    bool bDowned = false;
    bool bHasActiveWeapon = false;
};

/** 全境封锁 2 风格极简白色无文字背包侧边悬浮战术集群控件 */
UCLASS()
class ABBB_EVAC_API UBBBDivisionBackpackHud final : public UWidget
{
    GENERATED_BODY()

public:
    UBBBDivisionBackpackHud(const FObjectInitializer &ObjectInitializer);

    /**
     * 提交最新战斗展示快照 内部执行脏检查以最小化重绘开销
     * @param InSnapshot	本帧采集的玩家战斗状态快照
     * @return 无
     */
    void SetSnapshot(const FBBBDivisionHudSnapshot &InSnapshot);

    /** @return 当前是否正在显示弹药读数 */
    bool IsAmmoShown() const;

    /** @return 当前已显示的弹药数值 */
    int32 GetDisplayedLoadedAmmo() const;

    /** @return 当前已显示的弹匣容量数值 */
    int32 GetDisplayedCapacityAmmo() const;

    /** @return 当前主集群在视口中的平滑定位中心 */
    FVector2D GetSmoothedAnchorPosition() const;

protected:
    //~ Begin UWidget Interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void SynchronizeProperties() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UWidget Interface

private:
    /** 同步剪影材质池 仅在槽位图标纹理变化时更新材质参数 */
    void SyncSilhouetteMaterials(const FBBBDivisionHudSnapshot &InSnapshot);

    /** @param PoolIndex 池索引 @param Texture 目标图标 @return 槽位对应的只读画刷 */
    const FSlateBrush *UpdateSilhouetteSlot(int32 PoolIndex, UTexture2D *Texture);

    /** 物品白色剪影基础材质 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "物品剪影材质"))
    TObjectPtr<UMaterialInterface> SilhouetteMaterial;

    /** 几何主数字字体 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "主数字字体"))
    TObjectPtr<UFontFace> BoldNumberFont;

    /** 几何次级数字字体 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "次数字字体"))
    TObjectPtr<UFontFace> LightNumberFont;

    /** 纯白主前景颜色 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "主白颜色"))
    FLinearColor PrimaryTint = FLinearColor(0.97f, 0.98f, 1.0f, 0.96f);

    /** 次级霜白颜色 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "次白颜色"))
    FLinearColor SecondaryTint = FLinearColor(0.90f, 0.93f, 0.97f, 0.82f);

    /** 背后极克制暗色对比阴影强度 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "对比阴影强度", ClampMin = "0.0", ClampMax = "1.0"))
    float ContrastShadowStrength = 0.80f;

    /** 生命条分段数量 */
    UPROPERTY(EditAnywhere, Category = "外观", meta = (DisplayName = "血条分段数", ClampMin = "1", ClampMax = "8"))
    int32 HealthSegments = 5;

    /** 2.5D 透视纵向微倾斜率 */
    UPROPERTY(EditAnywhere, Category = "空间", meta = (DisplayName = "透视纵倾斜率", ClampMin = "-0.15", ClampMax = "0.15"))
    float PerspectiveSlopeY = -0.014f;

    /** 2.5D 透视横向微斜切 */
    UPROPERTY(EditAnywhere, Category = "空间", meta = (DisplayName = "透视横斜切", ClampMin = "-0.15", ClampMax = "0.15"))
    float PerspectiveShearX = -0.012f;

    /** 背包侧边相对屏幕微调偏移 */
    UPROPERTY(EditAnywhere, Category = "空间", meta = (DisplayName = "锚点屏幕微调"))
    FVector2D AnchorPixelOffset = FVector2D(18.0, 22.0);

    /** 瞄准时主集群的淡化不透明度 */
    UPROPERTY(EditAnywhere, Category = "空间", meta = (DisplayName = "瞄准淡化不透明度", ClampMin = "0.15", ClampMax = "1.0"))
    float AimDimOpacity = 0.42f;

    static constexpr int32 SilhouettePoolCount = 1 + FBBBDivisionHudSnapshot::MaxWeaponSlots + 1 + FBBBDivisionHudSnapshot::MaxSpareThrowables;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInstanceDynamic>> SilhouetteMaterials;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UTexture2D>> CachedTextures;

    FSlateBrush SilhouetteBrushes[SilhouettePoolCount];
    TSharedPtr<SBBBDivisionBackpackHud> SlateWidget;
    FBBBDivisionHudSnapshot LastSnapshot;
    bool bHasSnapshot = false;
};
