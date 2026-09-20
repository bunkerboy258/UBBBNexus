#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkFactLedgerState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkStates.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"

class APawn;
class ABBBCharacter;
class FBBBCharacterNetworkSystem;

UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /** 构造无 Tick 的角色网络传输组件 */
    UBBBCharacterNetworkComponent();

    /**
     * 绑定角色与事实账本接收边界
     *
     * @param InCharacter	网络数据所属角色
     */
    void Initialize(ABBBCharacter &InCharacter);

    /**
     * 注册只向模拟代理复制的最终状态与离散事实
     *
     * @param OutLifetimeProps	引擎复制属性列表
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBCharacterNetworkSystem;
    friend struct FBBBNetworkFactLedgerState;

    bool IsOwnerLocallyControlled() const;
    bool IsOwnerAuthority() const;

    /**
     * 将权威装备事实登记到增量账本
     *
     * @param Fact	已经成立的装备事实
     */
    void ReplicateEquipmentFact(FBBBEquipmentActionFact Fact);

    /**
     * 将权威装备最终状态复制给模拟代理
     *
     * @param EquipmentId	当前装备定义标识
     */
    void ReplicateEquipmentState(FName EquipmentId);

    /**
     * 将权威瞄准最终状态复制给模拟代理
     *
     * @param AimState	已经成立的瞄准状态
     */
    void ReplicateAimState(const FBBBAimNetworkState &AimState);

    /**
     * 将权威移动最终状态复制给模拟代理
     *
     * @param LocomotionState	已经成立的移动状态
     */
    void ReplicateLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    /**
     * 将接收的装备事实投递为领域输入
     *
     * @param Fact	已经成立的装备事实
     */
    void SubmitEquipmentFactInput(const FBBBEquipmentActionFact &Fact);

    /**
     * 将接收的装备状态投递为领域输入
     *
     * @param EquipmentId	当前装备定义标识
     */
    void SubmitEquipmentStateInput(FName EquipmentId);

    /**
     * 将接收的瞄准状态投递为领域输入
     *
     * @param AimState	已经成立的瞄准状态
     */
    void SubmitAimStateInput(const FBBBAimNetworkState &AimState);

    /**
     * 将接收的移动状态投递为领域输入
     *
     * @param LocomotionState	已经成立的移动状态
     */
    void SubmitLocomotionStateInput(const FBBBLocomotionNetworkState &LocomotionState);

    UFUNCTION(Server, Reliable)
    /**
     * 接收本机控制客户端已经形成的装备事实
     *
     * @param Fact	客户端本地已经成立的装备事实
     */
    void ServerSubmitEquipmentFact(FBBBEquipmentActionFact Fact);

    UFUNCTION(Server, Reliable)
    /**
     * 接收本机控制客户端已经形成的装备状态
     *
     * @param EquipmentId	客户端当前装备定义标识
     */
    void ServerSubmitEquipmentState(FName EquipmentId);

    UFUNCTION(Server, Unreliable)
    /**
     * 接收本机控制客户端最新瞄准状态
     *
     * @param AimState	客户端最新瞄准状态
     */
    void ServerSubmitAimState(FBBBAimNetworkState AimState);

    UFUNCTION(Server, Unreliable)
    /**
     * 接收本机控制客户端最新移动状态
     *
     * @param LocomotionState	客户端最新移动状态
     */
    void ServerSubmitLocomotionState(FBBBLocomotionNetworkState LocomotionState);

    UFUNCTION()
    /** 模拟代理收到装备状态时投递领域输入 */
    void OnRep_ReplicatedEquipmentId();

    UFUNCTION()
    /** 模拟代理收到瞄准状态时投递领域输入 */
    void OnRep_ReplicatedAimState();

    UFUNCTION()
    /** 模拟代理收到移动状态时投递领域输入 */
    void OnRep_ReplicatedLocomotionState();

    APawn *GetOwnerPawn() const;

    UPROPERTY(Replicated)
    /** 只复制给模拟代理的离散装备事实账本 */
    FBBBNetworkFactLedgerState ReplicatedFactLedger;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedEquipmentId)
    /** 只复制给模拟代理的当前装备最终状态 */
    FName ReplicatedEquipmentId = NAME_None;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    /** 只复制给模拟代理的瞄准最终状态 */
    FBBBAimNetworkState ReplicatedAimState;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
    /** 只复制给模拟代理的移动最终状态 */
    FBBBLocomotionNetworkState ReplicatedLocomotionState;

    /** 生命周期由角色持有 初始化后始终指向组件所属角色 */
    ABBBCharacter *Character = nullptr;
};
