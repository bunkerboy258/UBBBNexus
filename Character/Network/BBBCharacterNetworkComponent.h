#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBReplicatedAimState.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalAction.h"
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
     * 绑定角色复制边界
     * @param InCharacter 网络数据所属角色
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter);

    /**
     * 注册只向模拟代理复制的角色最终状态
     * @param OutLifetimeProps UE 复制属性列表
     * @return 无
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBEquipmentObservationProcessor;

    /** @param EquipmentId	实际装备标识 @param Generation	持有实例标识 @return 无 */
    void ReplicateEquipment(FName EquipmentId, uint64 Generation);

    /** @param EquipmentId	已经成立的装备标识 @param Generation	持有实例标识 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitEquipment(FName EquipmentId, uint64 Generation);

    /** @return 无 将当前持有关系提交为角色输入 */
    UFUNCTION()
    void OnRep_Equipment();

    /** 当前实际装备标识 */
    UPROPERTY(ReplicatedUsing = OnRep_Equipment)
    FName ReplicatedEquipmentId;

    /** 当前实际持有实例标识 */
    UPROPERTY(ReplicatedUsing = OnRep_Equipment)
    uint64 ReplicatedEquipmentGeneration = 0;

    friend class FBBBAimObservationProcessor;
    friend class FBBBRunObservationProcessor;
    friend class FBBBTraversalObservationProcessor;

    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    void ReplicateTraversal(uint32 Id, EBBBTraversalAction Action, const FTransform &Contact, const FTransform &End);
    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitTraversal(uint32 Id, EBBBTraversalAction Action, FTransform Contact, FTransform End);
    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastTraversal(uint32 Id, EBBBTraversalAction Action, FTransform Contact, FTransform End);

    /** @return 所属角色是否具有权威 */
    bool IsOwnerAuthority() const;

    /** @return 所属 Pawn 类型不匹配时返回空 */
    APawn *GetOwnerPawn() const;

    /**
     * 将权威瞄准最终状态复制给模拟代理
     * @param AimState 已经成立的瞄准状态
     * @return 无
     */
    void ReplicateAimState(const FBBBReplicatedAimState &AimState);

    /**
     * 将权威跑步状态复制给模拟代理
     * @param bRun 已经成立的跑步状态
     * @return 无
     */
    void ReplicateRunState(bool bRun);

    /**
     * 将接收的瞄准状态投递为领域输入
     * @param AimState 已经成立的瞄准状态
     * @return 无
     */
    void SubmitAimStateInput(const FBBBReplicatedAimState &AimState);

    /**
     * 将接收的跑步状态投递为领域输入
     * @param bRun 已经成立的跑步状态
     * @return 无
     */
    void SubmitRunStateInput(bool bRun);

    /**
     * 接收本机控制客户端最新瞄准状态
     * @param AimState 客户端最新瞄准状态
     * @return 无
     */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitAimState(FBBBReplicatedAimState AimState);

    /**
     * 接收本机控制客户端最新跑步状态
     * @param bRun 客户端最新跑步状态
     * @return 无
     */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitRunState(bool bRun);

    /** 模拟代理收到瞄准状态时投递领域输入 */
    UFUNCTION()
    void OnRep_ReplicatedAimState();

    /** 模拟代理收到跑步状态时投递领域输入 */
    UFUNCTION()
    void OnRep_ReplicatedRunState();

    /** 只复制给模拟代理的瞄准最终状态 */
    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    FBBBReplicatedAimState ReplicatedAimState;

    /** 只复制给模拟代理的当前跑步状态 */
    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedRunState)
    bool bReplicatedRun = false;

    /** 生命周期由角色持有 初始化后始终指向组件所属角色 */
    ABBBCharacter *Character = nullptr;
};
