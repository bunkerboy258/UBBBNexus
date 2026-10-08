#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBReplicatedAimState.h"
#include "Components/ActorComponent.h"
#include "Engine/NetSerialization.h"
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
    friend class FBBBCharacterRescueObservationProcessor;
    /** @return 无 分发当前救援结果 */
    void ReplicateRescue(APawn *Partner, uint64 Operation, uint64 Round, uint64 Revision,
        bool bHelping, bool bReceiving, bool bAccepted, float Elapsed, float Duration, FName Reason);
    /** @return 无 接收控制端既成救援结果 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitRescue(APawn *Partner, uint64 Operation, uint64 Round, uint64 Revision,
        bool bHelping, bool bReceiving, bool bAccepted, float Elapsed, float Duration, FName Reason);
    /** @return 无 通过发送者连接投送救援请求 */
    UFUNCTION(Server, Reliable)
    void ServerRequestRescue(ABBBCharacter *Target, uint64 Operation, uint64 Round);
    /** @return 无 向被救者控制端传递请求 */
    UFUNCTION(Client, Reliable)
    void ClientRequestRescue(APawn *Source, uint64 Operation, uint64 Round);
    /** @return 无 通过发送者连接投送取消事实 */
    UFUNCTION(Server, Reliable)
    void ServerEndRescue(ABBBCharacter *Target, uint64 Operation, uint64 Round);
    /** @return 无 向被救者控制端传递取消事实 */
    UFUNCTION(Client, Reliable)
    void ClientEndRescue(APawn *Source, uint64 Operation, uint64 Round);
    /** @return 无 通过被救者连接投送操作结果 */
    UFUNCTION(Server, Reliable)
    void ServerReplyRescue(ABBBCharacter *Target, uint64 Operation, uint64 Round,
        uint64 Revision, bool bActive, float Duration, FName Reason);
    /** @return 无 向救援者控制端传递操作结果 */
    UFUNCTION(Client, Reliable)
    void ClientReplyRescue(APawn *Source, uint64 Operation, uint64 Round,
        uint64 Revision, bool bActive, float Duration, FName Reason);
    /** @return 无 将当前救援结果转换为输入 */
    UFUNCTION()
    void OnRep_Rescue();
    /** 当前救援伙伴 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    TObjectPtr<APawn> ReplicatedRescuePartner;
    /** 当前救援操作 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    uint64 ReplicatedRescueOperation = 0;
    /** 当前救援倒地轮次 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    uint64 ReplicatedRescueRound = 0;
    /** 当前救援版本 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    uint64 ReplicatedRescueRevision = 0;
    /** 当前帮扶结果 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    bool bReplicatedRescueHelping = false;
    /** 当前接受帮扶结果 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    bool bReplicatedRescueReceiving = false;
    /** 当前请求已被接受 */
    UPROPERTY(ReplicatedUsing = OnRep_Rescue)
    bool bReplicatedRescueAccepted = false;
    /** 当前进度的服务器时间基准 */
    UPROPERTY(Replicated)
    float ReplicatedRescueStartTime = 0.0f;
    /** 当前救援时长 */
    UPROPERTY(Replicated)
    float ReplicatedRescueDuration = 3.0f;
    /** 最近结束原因 */
    UPROPERTY(Replicated)
    FName ReplicatedRescueReason;

    friend class FBBBCharacterLifeObservationProcessor;
    friend class FBBBCharacterDamageObservationProcessor;

    /** 复制已经成立的生命结果 */
    void ReplicateLife(EBBBCharacterLifePhase Phase, float Health, uint64 Revision,
        uint64 HitSerial, FName Bone, FVector Position, FVector Direction, uint64 DownedRevision, bool bRecoveryCrouched);

    /** 控制者上报自己的既成生命结果 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitLife(EBBBCharacterLifePhase Phase, float Health, uint64 Revision,
        uint64 HitSerial, FName Bone, FVector Position, FVector Direction, uint64 DownedRevision, bool bRecoveryCrouched);

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
    /** 当前倒地轮次 */
    UPROPERTY(Replicated)
    uint64 ReplicatedDownedRevision = 0;
    /** 最近恢复采用的蹲姿 */
    UPROPERTY(Replicated)
    bool bReplicatedRecoveryCrouched = false;
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
    friend class FBBBAccelerationObservationProcessor;

    /** 向镜像分发控制者已经产生的当前加速度 */
    void ReplicateAcceleration(uint64 Revision, const FVector &Acceleration, const FVector &MovementInput);

    /** 控制者上报当前加速度快照 不重演移动输入 */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitAcceleration(uint64 Revision, FVector_NetQuantize10 Acceleration, FVector_NetQuantize100 MovementInput);

    /** 新加速度版本到达后仅投递还原输入 */
    UFUNCTION()
    void OnRep_Acceleration();

    /** 与当前版本绑定的世界空间加速度 */
    UPROPERTY(Replicated)
    FVector_NetQuantize10 ReplicatedAcceleration = FVector::ZeroVector;

    /** 与当前加速度版本对应的已解析移动输入 */
    UPROPERTY(Replicated)
    FVector_NetQuantize100 ReplicatedMovementInput = FVector::ZeroVector;

    /** 当前快照版本用于拒绝乱序消息 */
    UPROPERTY(ReplicatedUsing = OnRep_Acceleration)
    uint64 ReplicatedAccelerationRevision = 0;
    friend class FBBBTraversalObservationProcessor;

    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    void ReplicateTraversal(uint32 Id, EBBBTraversalAction Action, const FTransform &Contact, const FTransform &End,
        float Position, const FVector &ExitVelocity);
    /** @param Id 动作序号 @param Action 动作结果 @param Contact 前沿目标 @param End 脚底目标 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitTraversal(uint32 Id, EBBBTraversalAction Action, FTransform Contact, FTransform End,
        float Position, FVector_NetQuantize10 ExitVelocity);
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

    /** 与结束动作标识一起还原的移动交权结果 */
    UPROPERTY(Replicated)
    FVector_NetQuantize10 ReplicatedTraversalExitVelocity = FVector::ZeroVector;

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
