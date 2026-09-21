#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBReplicatedAimState.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBReplicatedEquipmentFactArray.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"

class APawn;
class ABBBCharacter;

/** 角色 RPC 与属性复制边界 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /** 构造无 Tick 的角色网络传输组件 */
    UBBBCharacterNetworkComponent();

    /**
     * 绑定角色与装备事实复制数组
     * @param InCharacter 网络数据所属角色
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter);

    /**
     * 注册只向模拟代理复制的最终状态与离散事实
     * @param OutLifetimeProps UE 复制属性列表
     * @return 无
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBAimObservationProcessor;
    friend class FBBBEquipmentFactObservationProcessor;
    friend class FBBBEquipmentStateObservationProcessor;
    friend class FBBBLocomotionObservationProcessor;
    friend struct FBBBReplicatedEquipmentFactArray;

    /** @return 所属角色是否具有权威 */
    bool IsOwnerAuthority() const;

    /** @return 所属 Pawn 类型不匹配时返回空 */
    APawn *GetOwnerPawn() const;

    /**
     * 将权威装备事实登记到增量复制数组
     * @param Fact 已经成立的装备事实
     * @return 无
     */
    void ReplicateEquipmentFact(FBBBEquipmentActionFact Fact);

    /**
     * 将权威装备最终状态复制给模拟代理
     * @param EquipmentId 当前装备定义标识
     * @return 无
     */
    void ReplicateEquipmentState(FName EquipmentId);

    /**
     * 将权威瞄准最终状态复制给模拟代理
     * @param AimState 已经成立的瞄准状态
     * @return 无
     */
    void ReplicateAimState(const FBBBReplicatedAimState &AimState);

    /**
     * 将权威步态复制给模拟代理
     * @param Gait 已经成立的角色步态
     * @return 无
     */
    void ReplicateLocomotionState(EBBBCharacterGait Gait);

    /**
     * 将接收的装备事实投递为领域输入
     * @param Fact 已经成立的装备事实
     * @return 无
     */
    void SubmitEquipmentFactInput(const FBBBEquipmentActionFact &Fact);

    /**
     * 将接收的装备状态投递为领域输入
     * @param EquipmentId 当前装备定义标识
     * @return 无
     */
    void SubmitEquipmentStateInput(FName EquipmentId);

    /**
     * 将接收的瞄准状态投递为领域输入
     * @param AimState 已经成立的瞄准状态
     * @return 无
     */
    void SubmitAimStateInput(const FBBBReplicatedAimState &AimState);

    /**
     * 将接收的步态投递为领域输入
     * @param Gait 已经成立的角色步态
     * @return 无
     */
    void SubmitLocomotionStateInput(EBBBCharacterGait Gait);

    /**
     * 接收本机控制客户端已经形成的装备事实
     * @param Fact 客户端本地已经成立的装备事实
     * @return 无
     */
    UFUNCTION(Server, Reliable)
    void ServerSubmitEquipmentFact(FBBBEquipmentActionFact Fact);

    /**
     * 接收本机控制客户端已经形成的装备状态
     * @param EquipmentId 客户端当前装备定义标识
     * @return 无
     */
    UFUNCTION(Server, Reliable)
    void ServerSubmitEquipmentState(FName EquipmentId);

    /**
     * 接收本机控制客户端最新瞄准状态
     * @param AimState 客户端最新瞄准状态
     * @return 无
     */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitAimState(FBBBReplicatedAimState AimState);

    /**
     * 接收本机控制客户端最新步态
     * @param Gait 客户端最新步态
     * @return 无
     */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitLocomotionState(EBBBCharacterGait Gait);

    /** 模拟代理收到装备状态时投递领域输入 */
    UFUNCTION()
    void OnRep_ReplicatedEquipmentId();

    /** 模拟代理收到瞄准状态时投递领域输入 */
    UFUNCTION()
    void OnRep_ReplicatedAimState();

    /** 模拟代理收到步态时投递领域输入 */
    UFUNCTION()
    void OnRep_ReplicatedGait();

    /** 只复制给模拟代理的离散装备事实数组 */
    UPROPERTY(Replicated)
    FBBBReplicatedEquipmentFactArray ReplicatedEquipmentFacts;

    /** 只复制给模拟代理的当前装备最终状态 */
    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedEquipmentId)
    FName ReplicatedEquipmentId = NAME_None;

    /** 只复制给模拟代理的瞄准最终状态 */
    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    FBBBReplicatedAimState ReplicatedAimState;

    /** 只复制给模拟代理的当前步态 */
    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedGait)
    EBBBCharacterGait ReplicatedGait = EBBBCharacterGait::Run;

    /** 生命周期由角色持有 初始化后始终指向组件所属角色 */
    ABBBCharacter *Character = nullptr;
};
