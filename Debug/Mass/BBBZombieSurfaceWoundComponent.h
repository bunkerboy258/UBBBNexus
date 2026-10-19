#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBZombieSurfaceWoundComponent.generated.h"

class UBBBZombieGoreSurfaceDefinition;
class USkeletalMeshComponent;
class UTexture2D;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;
struct FBBBZombieGoreCutDefinition;

/** 单只调试靶的当前伤口遮罩和残肢表现 不维护玩法状态 */
UCLASS()
class ABBB_EVAC_API UBBBZombieSurfaceWoundComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    /** @return 不创建独立逐帧调度 */
    UBBBZombieSurfaceWoundComponent();

    /** @param Definition	独立调试资源 @return 建立身体材质与有限遮罩 */
    bool InitializeSurface(UBBBZombieGoreSurfaceDefinition& Definition);

    /** @param Hit	当前接触点 @param Strength	独立表现强度 @return 表面定位和写入是否成功 */
    bool Stamp(const FBBBMonsterHitReactionFragment& Hit, float Strength);

    /** @param Region	已损毁部位 @param Hit	触发命中 @param TierOverride	预览档位 零自动 @return 是否首次显示断点 */
    bool Sever(EBBBMonsterHitRegion Region, const FBBBMonsterHitReactionFragment& Hit, int32 TierOverride = 0);

    /** @param DeltaSeconds	当前帧间隔 @return 推进有限滴血和掉落回收 无返回值 */
    void Advance(float DeltaSeconds);

    /** @return 恢复当前身体并只清理本靶掉落部件 无返回值 */
    void ResetSurface();

    /** @param Blood	血污颜色 @param Tissue	组织颜色 @param Scale	边缘倍率 @param Roughness	组织粗糙度 @param Specular	组织高光 @param bVisible	伤口开关 @return 更新独立材质 无返回值 */
    void SetStyle(FLinearColor Blood, FLinearColor Tissue, float Scale, float Roughness, float Specular, bool bVisible);

    /** 基础破口半径 厘米 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|命中伤口", meta = (DisplayName = "基础伤口半径", ClampMin = "0.2", ClampMax = "10", Units = "cm"))
    float Radius = 1.5f;

    /** 同点反复命中使局部遮罩加深 受固定上限约束 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|命中伤口", meta = (DisplayName = "局部叠加强度", ClampMin = "0.05", ClampMax = "1"))
    float Accumulation = 0.4f;

    /** 独立于真实伤害与踉跄值 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|命中伤口", meta = (DisplayName = "默认伤口表现强度", ClampMin = "0.1", ClampMax = "4"))
    float DefaultStrength = 1.0f;

    /** 普通子弹偏夸张的掉落初速度 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断肢物理", meta = (DisplayName = "抛飞速度", ClampMin = "0", ClampMax = "1800", Units = "cm/s"))
    float LaunchSpeed = 650.0f;

    /** 向上分量避免所有断肢直接沿地滑落 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断肢物理", meta = (DisplayName = "向上速度", ClampMin = "0", ClampMax = "800", Units = "cm/s"))
    float LiftSpeed = 220.0f;

    /** 不使用无限转速或无约束软体 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断肢物理", meta = (DisplayName = "旋转速度", ClampMin = "0", ClampMax = "1200", Units = "deg/s"))
    float SpinSpeed = 420.0f;

    /** 本靶池满后回收最早掉落件 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断肢物理", meta = (DisplayName = "最大掉落部件数", ClampMin = "5", ClampMax = "24"))
    int32 MaximumPieces = 12;

    /** 超时只回收本靶部件 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断肢物理", meta = (DisplayName = "掉落部件保留时间", ClampMin = "1", ClampMax = "120", Units = "s"))
    float PieceLifetime = 30.0f;

    /** 有限滴血 不持续保存发射事件 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断口", meta = (DisplayName = "断口滴血时长", ClampMin = "0", ClampMax = "10", Units = "s"))
    float DripDuration = 2.0f;

    /** 最近有效命中投影到可见表面的距离 不是画面验收 */
    UPROPERTY(Transient, VisibleAnywhere, Category = "血腥调试|诊断", meta = (DisplayName = "最近表面定位偏差"))
    float LastProjectionError = 0;

    /** 当前固定大小的结果图集占用 */
    UPROPERTY(Transient, VisibleAnywhere, Category = "血腥调试|诊断", meta = (DisplayName = "伤口遮罩字节数"))
    int32 MaskBytes = 0;

    /** 六部位当前选定断点 零为完整 一为近端 二为远端 */
    UPROPERTY(Transient, VisibleAnywhere, Category = "血腥调试|诊断", meta = (DisplayName = "当前断点档位"))
    TArray<int32> SelectedTiers;

    /** @param EndPlayReason	关闭原因 @return 清理当前表现 无返回值 */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(Transient)
    TObjectPtr<UBBBZombieGoreSurfaceDefinition> Resources;
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Body;
    UPROPERTY(Transient)
    TObjectPtr<UTexture2D> Mask;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;
    /** 仅持有当前残肢和立体创口的材质 重置时释放 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInstanceDynamic>> DetailMaterials;
    UPROPERTY(Transient)
    TArray<TObjectPtr<USkeletalMeshComponent>> Caps;
    UPROPERTY(Transient)
    TArray<TObjectPtr<USkeletalMeshComponent>> Pieces;
    UPROPERTY(Transient)
    TObjectPtr<UStaticMeshComponent> HeavyWound;
    /** 当前躯干立体创口的表面三角形与重心坐标 不是命中事件列表 */
    FIntVector HeavyTriangle = FIntVector::ZeroValue;
    FVector HeavyBarycentric = FVector::ZeroVector;
    float HeavyRadius = 0;
    TArray<FColor> Pixels;
    TArray<float> DripRemaining;
    TArray<float> PieceAges;
    float DripClock = 0;
    bool bMaskDirty = false;
    /** @return 只上传当前结果 不保存修改过程 无返回值 */
    void UploadMask();
    /** @return 用当前蒙皮位置跟随立体创口 无返回值 */
    void UpdateHeavyWound();
};
