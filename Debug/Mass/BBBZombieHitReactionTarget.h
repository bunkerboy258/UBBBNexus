#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBZombieHitReactionTarget.generated.h"

class USkeletalMeshComponent;
class UBBBMonsterHitReactionComponent;
class UBBBMonsterPhysicalAnimationComponent;
class UBBBZombieHitTrajectoryComponent;

/** 仅用于本地受击调参的固定表现靶 不创建实体或维护生命与部位损毁 */
UCLASS(meta = (DisplayName = "僵尸程序化受击调参靶"))
class ABBB_EVAC_API ABBBZombieHitReactionTarget final : public AActor
{
    GENERATED_BODY()

public:
    /** @return 已配置独立物理资产和部位配置的调参靶 */
    ABBBZombieHitReactionTarget();

    /** @param RequestJson	独立运行时副本的采样配置 @return 启动结果 */
    UFUNCTION(BlueprintCallable, Category = "受击调参")
    FString StartHitTrajectory(const FString& RequestJson);

    /** @return 当前轨迹采样状态 */
    UFUNCTION(BlueprintCallable, Category = "受击调参")
    FString GetHitTrajectoryStatus() const;

    //~ Begin AActor Interface
    /**
     * 将标准子弹点命中注入正式受击表现组件 不扣血或触发断肢
     * @param DamageAmount	本次点伤害数值
     * @param DamageEvent	引擎命中数据
     * @param EventInstigator	射击者控制器
     * @param DamageCauser	射击来源
     * @return 零 不生成有效生命伤害
     */
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;
    //~ End AActor Interface

    /** 独立网格与物理资产 只影响当前调参靶 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "调参网格"))
    TObjectPtr<USkeletalMeshComponent> TargetMesh;

    /** 复用正式僵尸受击实现 四份配置仅引用调参目录 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "程序化受击"))
    TObjectPtr<UBBBMonsterHitReactionComponent> HitReaction;

    /** 复用正式物理驱动桥接 不启用移动或全身布娃娃 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "受击调参", meta = (DisplayName = "物理动画"))
    TObjectPtr<UBBBMonsterPhysicalAnimationComponent> PhysicalAnimation;

    /** 默认关闭的物理后轨迹诊断 */
    UPROPERTY(VisibleAnywhere, Category = "受击调参", meta = (DisplayName = "轨迹采样"))
    TObjectPtr<UBBBZombieHitTrajectoryComponent> Trajectory;

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
