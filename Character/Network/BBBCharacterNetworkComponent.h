#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
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
    friend class FBBBCharacterLifeObservationProcessor;
    friend class FBBBCharacterDamageObservationProcessor;

    /** 复制已经成立的生命结果 */
    void ReplicateLife(EBBBCharacterLifePhase Phase, float Health, uint64 Revision,
        uint64 HitSerial, FName Bone, FVector Position, FVector Direction);

    /** 控制者上报自己的既成生命结果 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitLife(EBBBCharacterLifePhase Phase, float Health, uint64 Revision,
        uint64 HitSerial, FName Bone, FVector Position, FVector Direction);

    /** 通过发送者拥有的连接投送命中 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitDamage(ABBBCharacter *Target, float Damage, FName Bone,
        FVector Position, FVector Direction, uint64 Sequence);

    /** 向目标自身的控制者投送独立命中 */
    UFUNCTION(Client, Reliable)
    void ClientDeliverDamage(float Damage, FName Bone, FVector Position, FVector Direction);

    /** 当前生命结果到达时提交还原输入 */
    UFUNCTION()
    void OnRep_Life();

    /** 当前阶段 */
    UPROPERTY(Replicated)
    EBBBCharacterLifePhase ReplicatedLifePhase = EBBBCharacterLifePhase::Alive;
    /** 当前生命 */
    UPROPERTY(Replicated)
    float ReplicatedHealth = 500.0f;
    /** 当前结果版本 */
    UPROPERTY(ReplicatedUsing = OnRep_Life)
    uint64 ReplicatedLifeRevision = 0;
    /** 最新命中序号 */
    UPROPERTY(Replicated)
    uint64 ReplicatedHitSerial = 0;
    /** 最新命中骨骼 */
    UPROPERTY(Replicated)
    FName ReplicatedHitBone;
    /** 最新命中位置 */
    UPROPERTY(Replicated)
    FVector ReplicatedHitPosition = FVector::ZeroVector;
    /** 最新命中方向 */
    UPROPERTY(Replicated)
    FVector ReplicatedHitDirection = FVector::ForwardVector;
    friend class FBBBEquipmentObservationProcessor;

    /** @param EquipmentId	实际装备标识 @param Generation	持有实例标识 @return 无 */
    void ReplicateEquipment(FName EquipmentId, uint64 Generation, bool bUsable, uint64 UseRevision);

    /** @param EquipmentId	已经成立的装备标识 @param Generation	持有实例标识 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitEquipment(FName EquipmentId, uint64 Generation, bool bUsable, uint64 UseRevision);

    /** @return 无 将当前持有关系提交为角色输入 */
    UFUNCTION()
    void OnRep_Equipment();

    /** 当前实际装备标识 */
    UPROPERTY(ReplicatedUsing = OnRep_Equipment)
    FName ReplicatedEquipmentId;

    /** 当前实际持有实例标识 */
    UPROPERTY(ReplicatedUsing = OnRep_Equipment)
    uint64 ReplicatedEquipmentGeneration = 0;

    /** 当前装备的独立使用许可 */
    UPROPERTY(ReplicatedUsing = OnRep_Equipment)
    bool bReplicatedEquipmentUsable = false;

    /** 当前装备独立使用结果版本 */
    UPROPERTY(ReplicatedUsing = OnRep_Equipment)
    uint64 ReplicatedEquipmentUseRevision = 0;

    friend class FBBBAimObservationProcessor;
    friend class FBBBRunObservationProcessor;
    friend class FBBBTraversalObservationProcessor;

    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    void ReplicateTraversal(uint32 Id, EBBBTraversalAction Action, const FTransform &Contact, const FTransform &End, float Position);
    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitTraversal(uint32 Id, EBBBTraversalAction Action, FTransform Contact, FTransform End, float Position);
    /** 当前攀爬结果到达后构造对应的角色输入 */
    UFUNCTION()
    void OnRep_Traversal();

    /** 当前动作序号 即使结束也保留以拒绝旧消息 */
    UPROPERTY(ReplicatedUsing = OnRep_Traversal)
    uint32 ReplicatedTraversalId = 0;

    /** 当前有效动作 结束时为无 */
    UPROPERTY(ReplicatedUsing = OnRep_Traversal)
    EBBBTraversalAction ReplicatedTraversalAction = EBBBTraversalAction::None;

    /** 与当前动作序号绑定的接触目标 */
    UPROPERTY(Replicated)
    FTransform ReplicatedTraversalContact = FTransform::Identity;

    /** 与当前动作序号绑定的落脚目标 */
    UPROPERTY(Replicated)
    FTransform ReplicatedTraversalEnd = FTransform::Identity;

    /** 服务器时间基准用于迟到观察者恢复当前进度 */
    UPROPERTY(Replicated)
    float ReplicatedTraversalStartTime = 0.0f;

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
