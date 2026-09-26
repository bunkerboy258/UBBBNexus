#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBAnimInstance.generated.h"

class FBBBCharacterAnimationActionProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBCharacterAnimationMontageProcessor;
class FBBBCharacterAnimationSystem;
class UBBBEquipmentAnimInstance;
class UAnimMontage;

namespace BBBCharacterMontageSlots
{
    inline const FName FullBody(TEXT("FullBody"));
    inline const FName UpperBody(TEXT("UpperBody"));
    inline const FName FullBodyAdditivePreAim(TEXT("FullBodyAdditivePreAim"));
    inline const FName UpperBodyAdditive(TEXT("UpperBodyAdditive"));
    inline const FName AdditiveHitReact(TEXT("AdditiveHitReact"));
}

/** 角色动画事实快照 蓝图读取与已批准蒙太奇执行入口 */
UCLASS()
class ABBB_EVAC_API UBBBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    friend class FBBBCharacterAnimationActionProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBCharacterAnimationMontageProcessor;
    friend class FBBBCharacterAnimationSystem;

public:
    /** @return 当前骨骼网格体上的 BBB 主动画实例 主实例自身调用时返回自身 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    UBBBAnimInstance *GetBBBMainAnimInstanceThreadSafe() const;

    /** 本帧角色世界位置 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceActorLocation = FVector::ZeroVector;

    /** 本帧角色世界旋转 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FRotator SourceActorRotation = FRotator::ZeroRotator;

    /** 本帧角色世界速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceVelocity = FVector::ZeroVector;

    /** 移动组件上一次更新后的世界速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceLastUpdateVelocity = FVector::ZeroVector;

    /** 本帧角色世界加速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    FVector SourceAcceleration = FVector::ZeroVector;

    /** 本帧引擎移动模式 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    TEnumAsByte<EMovementMode> SourceMovementMode = MOVE_None;

    /** 本帧地面摩擦力 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceGroundFriction = 0.0f;

    /** 本帧制动摩擦力 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceBrakingFriction = 0.0f;

    /** 本帧制动摩擦系数 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceBrakingFrictionFactor = 0.0f;

    /** 本帧行走制动减速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceBrakingDecelerationWalking = 0.0f;

    /** 是否使用独立制动摩擦力 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    bool bSourceUseSeparateBrakingFriction = false;

    /** 本帧重力加速度 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float SourceGravityZ = 0.0f;

    /** 本帧是否在地面移动 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    bool bSourceMovingOnGround = false;

    /** 本帧是否处于蹲伏 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    bool bSourceCrouching = false;

    /** @return 角色是否处于站立步行档位 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsWalking() const
    {
        const UBBBAnimInstance *Main = GetBBBMainAnimInstanceThreadSafe();
        return !Main->bSourceRunning && !Main->bSourceCrouching;
    }

    /** @return 角色是否处于站立跑步档位 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsRunning() const
    {
        const UBBBAnimInstance *Main = GetBBBMainAnimInstanceThreadSafe();
        return Main->bSourceRunning && !Main->bSourceCrouching;
    }

    /** @return 玩家是否具有瞄准意图 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool IsAiming() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->bSourceAiming;
    }

    /** @return 玩家瞄准意图的连续强度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIntentAlpha() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->SourceAimIntentAlpha;
    }

    /** @return 瞄准 IK 权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetAimIKAlpha() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->SourceAimIKAlpha;
    }

    /** @return 组件空间的瞄准目标点 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FVector GetAimTargetComponentSpace() const
    {
        return GetBBBMainAnimInstanceThreadSafe()->SourceAimTargetComponentSpace;
    }

    /** @return 当前装备的实际武器动画实例 链接层自动读取主实例绑定 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    UBBBEquipmentAnimInstance *TryGetWeaponAnimInstance() const;

    /** @return 枪口在角色 hand_r 骨骼空间中的快照变换 获取失败时返回单位变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform TryGetMuzzleTransformHandRSpace() const;

    /** @return 左手目标在右手骨骼空间中的位置 未装备时返回零向量 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FVector TryGetWeaponLeftHandTargetHandRSpace() const;

    /** @return 武器左手目标是否有效 未装备时返回 false */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool TryHasWeaponLeftHandTarget() const;



    /** 角色胶囊体底部到地面的距离 供动画属性存取节点直接读取 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float GroundDistance = 0.0f;

private:
    /**
     * 绑定装备实际使用的武器动画实例
     * @param InWeaponAnimInstance 武器动画实例 卸下时传入空
     * @return 无
     */
    void BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance);

    /**
     * 提交移动完成后的角色事实快照
     * @param FactState 新事实状态
     * @return 无
     */
    void PublishAnimationFacts(const FBBBCharacterAnimationFactState &FactState);

    /** @return 全部固定槽位蒙太奇贡献 */
    TStaticArray<TObjectPtr<UAnimMontage> *, 5> GetMontageContributions();

    /** @return 全部固定槽位蒙太奇贡献 */
    TStaticArray<const TObjectPtr<UAnimMontage> *, 5> GetMontageContributions() const;

    /**
     * 查找固定槽位蒙太奇贡献
     * @param SlotName 槽位名称
     * @return 对应贡献 未识别时返回空
     */
    TObjectPtr<UAnimMontage> *FindMontageContribution(FName SlotName);

    /**
     * 查找固定槽位蒙太奇贡献
     * @param SlotName 槽位名称
     * @return 对应贡献 未识别时返回空
     */
    const TObjectPtr<UAnimMontage> *FindMontageContribution(FName SlotName) const;

    /**
     * 注册已经批准的固定槽位蒙太奇贡献
     * @param SlotName 槽位名称
     * @param Montage 蒙太奇
     * @return 是否成功注册
     */
    bool RegisterMontageContribution(FName SlotName, UAnimMontage *Montage);

    /** 清除全部固定槽位蒙太奇贡献 */
    void ClearMontageContributions();

    /** 清除已经结束播放的固定槽位蒙太奇贡献 */
    void UpdateMontageContributions();

    UPROPERTY(Transient)
    bool bSourceRunning = false;

    UPROPERTY(Transient)
    bool bSourceAiming = false;

    UPROPERTY(Transient)
    float SourceAimIntentAlpha = 0.0f;

    UPROPERTY(Transient)
    float SourceAimIKAlpha = 0.0f;

    UPROPERTY(Transient)
    FVector SourceAimTargetComponentSpace = FVector::ZeroVector;

    /** 动画系统统一发布的枪口骨骼空间变换 */
    UPROPERTY(Transient)
    FTransform SourceMuzzleTransformHandRSpace = FTransform::Identity;

    /** 当前主手武器实际使用的动画实例 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBEquipmentAnimInstance> WeaponAnimInstance;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> FullBodyMontageContribution = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> UpperBodyMontageContribution = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> FullBodyAdditivePreAimMontageContribution = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> UpperBodyAdditiveMontageContribution = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> AdditiveHitReactMontageContribution = nullptr;
};
