#pragma once

#include "Components/ActorComponent.h"
#include "BBBMonsterGorePresentationComponent.generated.h"

class UBBBMonsterGoreDefinition;
class UBBBMonsterSoundPresentationDefinition;
class USkeletalMeshComponent;
class UMaterialInterface;
struct FBBBMonsterGoreFragment;
struct FBBBMonsterHitReactionFragment;

/** 唯一伤口呈现入口 只消费已成立的命中与损毁 不裁决生命 */
UCLASS(meta=(DisplayName="僵尸伤口表现"))
class ABBB_EVAC_API UBBBMonsterGorePresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /** @return 不注册逐实例逻辑更新 */
    UBBBMonsterGorePresentationComponent();

    UPROPERTY(Transient)
    TWeakObjectPtr<AActor> ResidueSource;

    /**
     * @param Result 当前有限表现状态
     * @param Definition 共享预制伤口配置
     * @param DestroyedParts 已成立的部位损毁
     * @param Hit 当前命中 不保存历史
     * @param Mesh 可选的姿态载体
     * @param Now 世界时间
     * @param bAuthority 是否允许选择永久外观
     * @return 无
     */
    static void UpdateWounds(FBBBMonsterGoreFragment& Result, const UBBBMonsterGoreDefinition& Definition,
        uint8 DestroyedParts, const FBBBMonsterHitReactionFragment& Hit, USkeletalMeshComponent* Mesh,
        float Now, bool bAuthority);

    /**
     * @param Definition 共享伤口配置
     * @param Facts 当前七处伤口
     * @param Hit 当前命中
     * @param Velocity 已成立速度
     * @param Seed 稳定出生种子
     * @param bRestore 是否只还原当前外观
     * @param Sounds 共享声音
     * @return 无
     */
    void Apply(const UBBBMonsterGoreDefinition& Definition, const FBBBMonsterGoreFragment& Facts,
        const FBBBMonsterHitReactionFragment& Hit, const FVector& Velocity, uint32 Seed, bool bRestore,
        UBBBMonsterSoundPresentationDefinition* Sounds = nullptr);

    /** @return 清除载体参数 不改变实体伤口 */
    void ResetPresentation();

    /** @return 当前绑定的主身体 不包含组织跟随组件 */
    const USkeletalMeshComponent* GetBody() const { return Body.Get(); }

    /** @param EndPlayReason 关闭原因 @return 回收表现载体 */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Body;

    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Tissue[7];

    UPROPERTY(Transient)
    TObjectPtr<const UBBBMonsterGoreDefinition> AppliedDefinition;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

    /** 仅去重短时掉落 不作为损毁事实 */
    uint8 AppliedPresets[7] = {};
    bool bUsingDamageMaterials = false;

    /** @param Definition 共享配置 @return 是否准备完成 */
    bool Prepare(const UBBBMonsterGoreDefinition& Definition);
};
