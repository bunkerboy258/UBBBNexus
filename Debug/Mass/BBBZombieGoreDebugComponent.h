#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBZombieGoreDebugComponent.generated.h"

class UBBBZombieGoreSurfaceDefinition;
class UBBBMonsterBloodPresentationDefinition;

/** 单只调试靶的当前血腥表现 不创建实体或维护玩法生命 */
UCLASS(meta = (DisplayName = "血腥表现调试"))
class ABBB_EVAC_API UBBBZombieGoreDebugComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    /** @return 创建仅本地调试使用的表现推进组件 */
    UBBBZombieGoreDebugComponent();

    /** 独立的调试伤口和断肢资源 不能指定正式外观配置 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|配置", meta = (DisplayName = "伤口资源配置"))
    TObjectPtr<UBBBZombieGoreSurfaceDefinition> AppearanceDefinition;

    /** 独立的血滴与环境残留配置 修改后重新开始游戏 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|配置", meta = (DisplayName = "喷血和残留配置"))
    TObjectPtr<UBBBMonsterBloodPresentationDefinition> BloodSettings;

    /** 零表示不自动恢复 从周期开始计时 不因连续命中延后 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|重置", meta = (DisplayName = "自动重置间隔", ClampMin = "0", Units = "s", ToolTip = "默认每3秒恢复完整身体 设为0关闭 不清除场景中其他僵尸的表现"))
    float ResetInterval = 3.0f;

    /** 只清理此靶产生的落地血滴和环境血迹 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|重置", meta = (DisplayName = "重置时清理本靶环境血迹"))
    bool bClearResidueOnReset = false;

    /** 每次有效命中增加对应部位的调试比例 不计算真实生命 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|命中", meta = (DisplayName = "每发受损增量", ClampMin = "0", ClampMax = "1"))
    float DamageStep = 0.25f;

    /** 满比例时允许还原断开表现 调试靶本身不死亡 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|命中", meta = (DisplayName = "允许断肢预览"))
    bool bAllowSevering = true;

    /** 可关闭以只查看喷血或伤口 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|开关", meta = (DisplayName = "启用程序化受击"))
    bool bEnablePhysicalReaction = true;

    /** 仅控制普通命中的血效提交 断肢血效使用同一配置的开关 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|开关", meta = (DisplayName = "启用命中喷血和落地血滴"))
    bool bEnableHitBlood = true;

    /** 身体表面的分区血污 不影响地面血迹 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|身体", meta = (DisplayName = "显示身体血污"))
    bool bShowBloodStains = true;

    /** 胸腹预制创口与组织表面 不代表逐弹精确弹孔 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|身体", meta = (DisplayName = "显示创口"))
    bool bShowWounds = true;

    /** 当前血效的稳定随机种子 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|身体", meta = (DisplayName = "血效随机种子", ClampMin = "0"))
    int32 PatternSeed = 17;

    /** 线性颜色空间下的身体血污基色 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|身体", meta = (DisplayName = "血污颜色"))
    FLinearColor BloodTint = FLinearColor(0.12f, 0.003f, 0.007f);

    /** 放大或减弱已有分区血污覆盖 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|身体", meta = (DisplayName = "血污强度", ClampMin = "0", ClampMax = "3"))
    float BloodStrength = 1;

    /** 创口肌肉组织基色 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|伤口", meta = (DisplayName = "组织颜色"))
    FLinearColor TissueTint = FLinearColor(0.055f, 0.002f, 0.004f);

    /** 组织表面粗糙度 越低越光滑 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|伤口", meta = (DisplayName = "组织粗糙度", ClampMin = "0", ClampMax = "1"))
    float TissueRoughness = 0.66f;

    /** 组织表面的高光响应 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|伤口", meta = (DisplayName = "组织高光", ClampMin = "0", ClampMax = "1"))
    float TissueSpecular = 0.12f;

    /** 手动预览的目标部位 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|手动预览", meta = (DisplayName = "预览部位"))
    EBBBMonsterHitRegion PreviewRegion = EBBBMonsterHitRegion::Torso;

    /** 一为近端 二为远端 只影响手动预览 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|手动预览", meta = (DisplayName = "预览断点档位", ClampMin = "1", ClampMax = "2"))
    int32 PreviewCutTier = 2;

    /** 当前六部位的调试受损程度 顺序为躯干 头 左臂 右臂 左腿 右腿 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "血腥调试|诊断", meta = (DisplayName = "当前六部位受损比例"))
    TArray<float> DamageRatios;

    /** 只计当前游戏实例的恢复次数 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "血腥调试|诊断", meta = (DisplayName = "已重置次数"))
    int32 ResetCount = 0;

    /** @return 恢复完整身体和局部物理 不清除其他对象 无返回值 */
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "血腥调试|操作", meta = (DisplayName = "立即重置僵尸"))
    void ResetTarget();

    /** @return 还原选定非躯干部位断开 不杀死调试靶 无返回值 */
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "血腥调试|操作", meta = (DisplayName = "预览选定部位断开"))
    void PreviewSevering();

    /** @return 清除此靶的飞行血滴和环境残留 无返回值 */
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "血腥调试|操作", meta = (DisplayName = "清理本靶环境血迹"))
    void ClearBloodResidue();

    /**
     * @param Hit\t靶子已接受的当前点命中
     * @return 无返回值
     */
    void ApplyHit(const FBBBMonsterHitReactionFragment& Hit, float WoundStrength = 1.0f);

    //~ Begin UActorComponent Interface
    /** @return 建立独立的当前调试资源副本 无返回值 */
    virtual void BeginPlay() override;
    /**
     * @param DeltaTime\t当前帧间隔
     * @param TickType\t引擎更新类型
     * @param ThisTickFunction\t本组件调度句柄
     * @return 推进本地表现及重置周期 无返回值
     */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    /**
     * @param EndPlayReason\t引擎结束原因
     * @return 清理本靶残留引用 无返回值
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End UActorComponent Interface
private:
    /** 资源初始化成功后才允许更新当前表现 */
    bool bSurfaceReady = false;
    /** 最近有效命中只用于当前表现 */
    FBBBMonsterHitReactionFragment CurrentHit;
    /** 当前恢复周期的截止世界时间 */
    double NextResetTime = 0;
    /** 检测用户是否调整恢复间隔 不保存历次设置 */
    float AppliedResetInterval = -1;
    /** 检测断肢预览开关变化以恢复被停用的局部物理 */
    bool bAppliedAllowSevering = true;
    /** @return 更新伤口组件和本靶材质的当前参数 无返回值 */
    void ApplyCurrentWounds();
};
