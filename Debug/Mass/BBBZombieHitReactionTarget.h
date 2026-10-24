#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBZombieHitReactionTarget.generated.h"

class USkeletalMeshComponent;
class UBBBMonsterHitReactionComponent;
class UBBBMonsterPhysicalAnimationComponent;
class UBBBZombieHitTrajectoryComponent;
class UBBBMonsterGorePreviewComponent;
class UBBBMonsterGorePresentationComponent;
class UBBBMonsterPresentationComponent;
class UMassEntityConfigAsset;
class UPhysicsAsset;
class UHitReactProfile;
class UAnimSequence;
class UBBBMonsterGoreDefinition;
class UBBBMonsterPresentationDefinition;

/** 仅用于本地受击与血腥调参的固定表现靶 不创建实体或维护玩法生命 */
UCLASS(meta = (DisplayName = "僵尸程序化受击调参靶"))
class ABBB_EVAC_API ABBBZombieHitReactionTarget final : public AActor
{
    GENERATED_BODY()

public:
    /** @return 通过正式模板配置的固定表现靶 */
    ABBBZombieHitReactionTarget();

    /** 正式模板是模型 动画 物理与伤口引用的唯一默认来源 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试僵尸", meta = (DisplayName = "僵尸版本"))
    TObjectPtr<UMassEntityConfigAsset> ZombieVersion;

    /** 留空使用正式动画蓝图 指定时只播放该段兼容动画 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试僵尸", meta = (DisplayName = "测试动画", ToolTip = "留空使用所选版本的正式待机 轨迹采样时需指定动画序列"))
    TObjectPtr<UAnimSequence> TestAnimation;

    /** 用一份完整测试表现配置替代正式表现 不另设分散覆盖 */
    UPROPERTY(EditAnywhere, Category = "测试僵尸", meta = (DisplayName = "使用自定义表现"))
    bool bOverridePresentation = false;

    UPROPERTY(EditAnywhere, Category = "测试僵尸", meta = (DisplayName = "自定义表现配置", EditCondition = "bOverridePresentation"))
    TObjectPtr<UBBBMonsterPresentationDefinition> PresentationOverride;

    /** 当前配置失败时隐藏身体并禁止命中 不悄悄使用上一版本 */
    UPROPERTY(Transient, VisibleAnywhere, Category = "测试僵尸", meta = (DisplayName = "配置状态"))
    FString ConfigurationStatus;

    /** 当前选择已经完整解析并可以用于命中 */
    UPROPERTY(Transient, VisibleAnywhere, Category = "测试僵尸", meta = (DisplayName = "配置有效"))
    bool bConfigurationReady = false;

    /** @return 重新解析全部配置并清除上一版本的表现 */
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "测试僵尸", meta = (DisplayName = "应用版本与覆盖"))
    bool ApplyConfiguration();

    /** @return 更新正式动画蓝图需要的本地预览快照 无返回值 */
    void UpdatePreviewAnimation();

    /** @param RequestJson	独立运行时副本的采样配置 @return 启动结果 */
    UFUNCTION(BlueprintCallable, Category = "受击调参")
    FString StartHitTrajectory(const FString& RequestJson);

    /** @return 当前轨迹采样状态 */
    UFUNCTION(BlueprintCallable, Category = "受击调参")
    FString GetHitTrajectoryStatus() const;

    //~ Begin AActor Interface
    /**
     * 将标准子弹点命中注入正式受击与血腥表现 不产生玩法生命伤害
     * @param DamageAmount	本次点伤害数值
     * @param DamageEvent	引擎命中数据
     * @param EventInstigator	射击者控制器
     * @param DamageCauser	射击来源
     * @return 零 不生成有效生命伤害
     */
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
    //~ End AActor Interface

    /** 独立网格与物理资产 只影响当前调参靶 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "调参网格"))
    TObjectPtr<USkeletalMeshComponent> TargetMesh;

    /** 复用正式僵尸受击实现 配置由版本或明确覆盖提供 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "程序化受击"))
    TObjectPtr<UBBBMonsterHitReactionComponent> HitReaction;

    /** 复用正式物理驱动桥接 不启用移动或全身布娃娃 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "物理动画"))
    TObjectPtr<UBBBMonsterPhysicalAnimationComponent> PhysicalAnimation;

    /** 默认关闭的物理后轨迹诊断 */
    UPROPERTY(VisibleAnywhere, Category = "受击调参", meta = (DisplayName = "轨迹采样"))
    TObjectPtr<UBBBZombieHitTrajectoryComponent> Trajectory;

    /** 独立的血腥表现调节与自动恢复 不维护玩法生命 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "血腥表现调试"))
    TObjectPtr<UBBBMonsterGorePreviewComponent> GorePreview;

    /** 仅还原调试受损结果 */
    UPROPERTY(VisibleAnywhere, Category = "受击调参", meta = (DisplayName = "伤口断肢表现"))
    TObjectPtr<UBBBMonsterGorePresentationComponent> Gore;

    /** 只提供固定待机与循环相位 不创建 Mass 实体或玩法状态 */
    UPROPERTY(VisibleAnywhere, Category = "受击调参", meta = (DisplayName = "正式动画桥接"))
    TObjectPtr<UBBBMonsterPresentationComponent> AnimationPreview;

    /** 当前游戏实例收到的有效点命中数量 不保存历史 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "受击调参", meta = (DisplayName = "命中次数"))
    uint32 HitCount = 0;

    /** 最近点命中的原始骨骼 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "受击调参", meta = (DisplayName = "最近命中骨骼"))
    FName LastHitBone;

    /** 最近命中使用的正式受击部位 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "受击调参", meta = (DisplayName = "最近命中部位"))
    EBBBMonsterHitRegion LastHitRegion = EBBBMonsterHitRegion::Torso;
};
