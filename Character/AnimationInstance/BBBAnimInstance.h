#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBAnimInstance.generated.h"

class FBBBCharacterAnimationActionProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterInputProcessor;
class UBBBEquipmentAnimInstance;
class UAnimMontage;
struct FBBBMontagePacketData;

namespace BBBCharacterMontageSlots
{
    inline const FName FullBody(TEXT("FullBody"));
    inline const FName UpperBody(TEXT("UpperBody"));
    inline const FName FullBodyAdditivePreAim(TEXT("FullBodyAdditivePreAim"));
    inline const FName UpperBodyAdditive(TEXT("UpperBodyAdditive"));
    inline const FName AdditiveHitReact(TEXT("AdditiveHitReact"));
}

/** 动画实例内部单个固定蒙太奇槽位的完整执行状态 */
USTRUCT()
struct FBBBCharacterMontageSlot final
{
    GENERATED_BODY()

    /** 已经批准播放的蒙太奇 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** 已经批准的播放倍率 */
    UPROPERTY(Transient)
    float PlayRate = 1.0f;

    /** 请求所属操作序号 */
    UPROPERTY(Transient)
    int32 Sequence = INDEX_NONE;

    /** 多槽位共享的播放修订号 */
    uint64 Revision = 0;

    /** 请求是否属于换弹流程 */
    UPROPERTY(Transient)
    bool bReload = false;

    /** 请求是否已经提交给引擎 */
    bool bSubmitted = false;
};

/** 角色动画事实快照 蓝图读取与已批准蒙太奇执行入口 */
UCLASS()
class ABBB_EVAC_API UBBBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    friend class FBBBCharacterAnimationActionProcessor;
    friend class FBBBCharacterAnimationSystem;
    friend class FBBBCharacterInputProcessor;
    friend struct FBBBMontagePacketData;

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

    /** @return BBBNexus 本地计算或网络恢复后的精确步态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    EBBBCharacterGait GetGait() const
    {
        return SourceGait;
    }

    /** @return 精确步态是否为行走 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsWalking() const
    {
        return SourceGait == EBBBCharacterGait::Walk;
    }

    /** @return 精确步态是否为奔跑 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsRunning() const
    {
        return SourceGait == EBBBCharacterGait::Run;
    }

    /** @return 精确步态是否为冲刺 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsSprinting() const
    {
        return SourceGait == EBBBCharacterGait::Sprint;
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

    /** @return 武器瞄准来源本地变换 未装备时返回单位变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FTransform TryGetWeaponAimSourceLocalTransform() const;

    /** @return 左手目标在右手骨骼空间中的位置 未装备时返回零向量 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FVector TryGetWeaponLeftHandTargetHandRSpace() const;

    /** @return 武器左手目标是否有效 未装备时返回 false */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool TryHasWeaponLeftHandTarget() const;

    /** @return 当前武器是否正在换弹 未装备时返回 false */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool TryGetWeaponReloading() const;

    /** @return 距离当前武器上次开火的秒数 未装备时返回极大值 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    float TryGetWeaponTimeSinceLastFireSeconds() const;

    /**
     * 绑定装备实际使用的武器动画实例
     * @param InWeaponAnimInstance 武器动画实例 卸下时传入空
     * @return 无
     */
    void BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance);

    /** 角色胶囊体底部到地面的距离 供动画属性存取节点直接读取 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Animation Facts")
    float GroundDistance = 0.0f;

private:
    /**
     * 提交移动完成后的角色事实快照
     * @param FactState 新事实状态
     * @return 无
     */
    void PublishAnimationFacts(const FBBBCharacterAnimationFactState &FactState);

    /** @return 全部固定蒙太奇槽位 */
    TStaticArray<FBBBCharacterMontageSlot *, 5> GetMontageSlots();

    /** @return 全部固定蒙太奇槽位 */
    TStaticArray<const FBBBCharacterMontageSlot *, 5> GetMontageSlots() const;

    /**
     * 查找固定蒙太奇槽位
     * @param SlotName 槽位名称
     * @return 对应槽位 未识别时返回空
     */
    FBBBCharacterMontageSlot *FindMontageSlot(FName SlotName);

    /**
     * 查找固定蒙太奇槽位
     * @param SlotName 槽位名称
     * @return 对应槽位 未识别时返回空
     */
    const FBBBCharacterMontageSlot *FindMontageSlot(FName SlotName) const;

    /**
     * 写入已经批准的固定槽位蒙太奇请求
     * @param SlotName 槽位名称
     * @param Montage 蒙太奇
     * @param PlayRate 播放倍率
     * @param Sequence 操作序号
     * @param bReload 是否属于换弹
     * @return 是否成功写入
     */
    bool SubmitMontageSlot(
        FName SlotName,
        UAnimMontage &Montage,
        float PlayRate,
        int32 Sequence,
        bool bReload);

    /**
     * 清除已经失效的槽位请求
     * @param bEquipmentSwitchPending 是否等待装备切换
     * @param CancelledReloadSequence 已取消换弹序号
     * @return 无
     */
    void InvalidateMontageSlots(bool bEquipmentSwitchPending, int32 CancelledReloadSequence);

    /** 执行并维护已经批准的蒙太奇请求 */
    void UpdateApprovedMontages();

    /**
     * 清除共享修订号的全部槽位
     * @param Revision 修订号
     * @param bStopMontage 是否停止引擎中的蒙太奇
     * @return 无
     */
    void ClearMontageRevision(uint64 Revision, bool bStopMontage);

    /**
     * 释放单个槽位并在最后引用消失时停止播放
     * @param Slot 需要释放的槽位
     * @return 无
     */
    void ReleaseMontageSlot(FBBBCharacterMontageSlot &Slot);

    UPROPERTY(Transient)
    EBBBCharacterGait SourceGait = EBBBCharacterGait::Run;

    UPROPERTY(Transient)
    bool bSourceAiming = false;

    UPROPERTY(Transient)
    float SourceAimIntentAlpha = 0.0f;

    UPROPERTY(Transient)
    float SourceAimIKAlpha = 0.0f;

    UPROPERTY(Transient)
    FVector SourceAimTargetComponentSpace = FVector::ZeroVector;

    /** 当前主手武器实际使用的动画实例 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBEquipmentAnimInstance> WeaponAnimInstance;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlot FullBodyMontageSlot;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlot UpperBodyMontageSlot;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlot FullBodyAdditivePreAimMontageSlot;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlot UpperBodyAdditiveMontageSlot;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlot AdditiveHitReactMontageSlot;

    uint64 NextMontageRevision = 1;
};
