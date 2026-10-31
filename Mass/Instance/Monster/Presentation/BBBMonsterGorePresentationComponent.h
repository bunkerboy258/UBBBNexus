#pragma once

#include "Components/ActorComponent.h"
#include "BBBMonsterGorePresentationComponent.generated.h"

class UBBBMonsterGoreDefinition;
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
     * @param Definition 共享伤口配置
     * @param Facts 当前八区毁伤和五处断肢
     * @param Hit 当前命中
     * @param Velocity 已成立速度
     * @param Seed 稳定出生种子
     * @param InstanceId	当前实体出生身份 用于碎肉限频
     * @param bRestore 是否只还原当前外观
     * @return 无
     */
    void Apply(const UBBBMonsterGoreDefinition& Definition, const FBBBMonsterGoreFragment& Facts,
        const FBBBMonsterHitReactionFragment& Hit, const FVector& Velocity, uint32 Seed, const FGuid& InstanceId, bool bRestore);

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
    TObjectPtr<USkeletalMeshComponent> Tissue[6];

    UPROPERTY(Transient)
    TObjectPtr<const UBBBMonsterGoreDefinition> AppliedDefinition;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

    /** 仅去重短时掉落 不作为损毁事实 */
    uint8 AppliedPresets[5] = {};
    bool bUsingDamageMaterials = false;

    /** @param Definition 共享配置 @return 是否准备完成 */
    bool Prepare(const UBBBMonsterGoreDefinition& Definition);
};
