#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Interaction/FBBBCharacterUseRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Interaction/FBBBCharacterUseAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Aim/FBBBAimStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBRunStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBAccelerationAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Locomotion/FBBBAccelerationRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Interaction/FBBBCharacterInteractionSelectionRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Interaction/FBBBCharacterInteractionSelectionAuthorityFactPacket.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Appearance/FBBBCharacterAppearanceRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Appearance/FBBBCharacterAppearanceAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueRequestLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueReplyLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueRequestRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueReplyRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueSnapshotRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterRescueSnapshotAuthorityFactPacket.h"

#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterLifeAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterLifeRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterDamageDeliveryRemoteMessagePacket.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedUse, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAppearance, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAcceleration, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedMovementInput, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAccelerationRevision, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLifePhase, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHealth, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLifeRevision, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitSerial, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedDownedRevision, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, bReplicatedRecoveryCrouched, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitBone, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitPosition, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitDirection, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedInteractionId, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedInteractionGeneration, COND_SimulatedOnly);


    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescuePartner, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescueOperation, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescueRound, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescueRevision, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, bReplicatedRescueHelping, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, bReplicatedRescueReceiving, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, bReplicatedRescueAccepted, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescueStartTime, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescueDuration, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedRescueReason, COND_SimulatedOnly);

    // 本机控制角色已经生成同一份事实 只让模拟代理执行接收投递
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedAimState,
        COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        bReplicatedRun,
        COND_SimulatedOnly);
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(
    FBBBReplicatedAimState AimState)
{
    // 连续状态允许覆盖 但非法向量不能污染角色输入
    if (!ensureMsgf(
        Character
            && IsOwnerAuthority()
            && !FVector(AimState.AimTargetWorld).ContainsNaN(),
        TEXT("瞄准状态网络投递被拒绝")))
    {
        return;
    }

    SubmitAimStateInput(AimState);
}

void UBBBCharacterNetworkComponent::ServerSubmitRunState_Implementation(const bool bRun)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(), TEXT("跑步状态网络投递被拒绝")))
    {
        return;
    }

    SubmitRunStateInput(bRun);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    SubmitAimStateInput(ReplicatedAimState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedRunState()
{
    SubmitRunStateInput(bReplicatedRun);
}

void UBBBCharacterNetworkComponent::ReplicateAcceleration(
    const uint64 Revision, const FVector &Acceleration, const FVector &MovementInput)
{
    if (!IsOwnerAuthority() || Revision <= ReplicatedAccelerationRevision)
    {
        return;
    }
    ReplicatedAcceleration = Acceleration;
    ReplicatedMovementInput = MovementInput;
    ReplicatedAccelerationRevision = Revision;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitAcceleration_Implementation(
    const uint64 Revision, const FVector_NetQuantize10 Acceleration, const FVector_NetQuantize100 MovementInput)
{
    if (!Character || !IsOwnerAuthority() || Character->IsLocallyControlled()
        || Revision <= ReplicatedAccelerationRevision)
    {
        return;
    }
    // 只投递已经完成的移动事实 结构和版本校验交由输入包处理
    FBBBAccelerationRemoteMessagePacket Packet{{Revision}, {FVector(Acceleration)}, {FVector(MovementInput)}};
    if (Packet.IsValid())
    {
        Character->SubmitInput(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::OnRep_Acceleration()
{
    if (!Character)
    {
        Character = Cast<ABBBCharacter>(GetOwner());
    }
    if (Character && !IsOwnerAuthority() && !Character->IsLocallyControlled()
        && ReplicatedAccelerationRevision != 0)
    {
        Character->SubmitInput(FBBBAccelerationAuthorityFactPacket{
            {ReplicatedAccelerationRevision}, {FVector(ReplicatedAcceleration)}, {FVector(ReplicatedMovementInput)}});
    }
}

void UBBBCharacterNetworkComponent::ReplicateAimState(
    const FBBBReplicatedAimState &AimState)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    ReplicatedAimState = AimState;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateRunState(const bool bRun)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    bReplicatedRun = bRun;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::SubmitAimStateInput(
    const FBBBReplicatedAimState &AimState)
{
    if (!Character)
    {
        return;
    }

    FBBBAimStateAuthorityFactPacket Packet;
    Packet.bIsAiming = AimState.bIsAiming;
    Packet.bFire = AimState.bFire;
    Packet.AimTargetWorld = AimState.AimTargetWorld;
    Character->SubmitInput(MoveTemp(Packet));
}

void UBBBCharacterNetworkComponent::SubmitRunStateInput(const bool bRun)
{
    if (Character)
    {
        Character->SubmitInput(FBBBRunStateAuthorityFactPacket{bRun});
    }
}

bool UBBBCharacterNetworkComponent::IsOwnerAuthority() const
{
    const APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->HasAuthority();
}

APawn *UBBBCharacterNetworkComponent::GetOwnerPawn() const
{
    return Cast<APawn>(GetOwner());
}

void UBBBCharacterNetworkComponent::ReplicateInteraction(const FName EquipmentId, const uint64 Generation)
{
    if (!IsOwnerAuthority() || Generation == 0 || Generation < ReplicatedInteractionGeneration)
    {
        return;
    }

    ReplicatedInteractionId = EquipmentId;
    ReplicatedInteractionGeneration = Generation;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitInteraction_Implementation(const FName EquipmentId, const uint64 Generation)
{
    if (Character && IsOwnerAuthority() && Generation > 0 && Generation > ReplicatedInteractionGeneration)
    {
        Character->SubmitInput(FBBBCharacterInteractionSelectionRemoteMessagePacket{{EquipmentId}, {Generation}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Interaction()
{
    if (Character && Character->IsNetworkMirror() && ReplicatedInteractionGeneration > 0)
    {
        Character->SubmitInput(FBBBCharacterInteractionSelectionAuthorityFactPacket{{ReplicatedInteractionId}, {ReplicatedInteractionGeneration}});

    }
}

void UBBBCharacterNetworkComponent::ReplicateLife(EBBBCharacterLifePhase Phase, float Health,
    uint64 Revision, uint64 HitSerial, FName Bone, FVector Position, FVector Direction, uint64 DownedRevision, bool bRecoveryCrouched)
{
    ReplicatedLifePhase = Phase;
    ReplicatedHealth = Health;
    ReplicatedLifeRevision = Revision;
    ReplicatedDownedRevision = DownedRevision;
    bReplicatedRecoveryCrouched = bRecoveryCrouched;
    ReplicatedHitSerial = HitSerial;
    ReplicatedHitBone = Bone;
    ReplicatedHitPosition = Position;
    ReplicatedHitDirection = Direction;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitLife_Implementation(EBBBCharacterLifePhase Phase, float Health,
    uint64 Revision, uint64 HitSerial, FName Bone, FVector Position, FVector Direction, uint64 DownedRevision, bool bRecoveryCrouched)
{
    if (Character)
    {
        Character->SubmitInput(FBBBCharacterLifeRemoteMessagePacket{
            {Phase}, {Health}, {Revision}, {HitSerial}, {Bone}, {Position}, {Direction}, {DownedRevision}, {bRecoveryCrouched}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Life()
{
    if (!Character) Character = Cast<ABBBCharacter>(GetOwner());
    if (Character && ReplicatedLifeRevision > 0)
    {
        Character->SubmitInput(FBBBCharacterLifeAuthorityFactPacket{
            {ReplicatedLifePhase}, {ReplicatedHealth}, {ReplicatedLifeRevision},
            {ReplicatedHitSerial}, {ReplicatedHitBone}, {ReplicatedHitPosition}, {ReplicatedHitDirection}, {ReplicatedDownedRevision}, {bReplicatedRecoveryCrouched}});
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitDamage_Implementation(ABBBCharacter *Target,
    float Damage, FName Bone, FVector Position, FVector Direction, uint64 Sequence)
{
    if (IsValid(Target) && Character)
    {
        Target->SubmitInput(FBBBCharacterDamageDeliveryRemoteMessagePacket{
            {Damage}, {Bone}, {Position}, {Direction}, {Character}, {Sequence}});
    }
}

void UBBBCharacterNetworkComponent::ClientDeliverDamage_Implementation(float Damage,
    FName Bone, FVector Position, FVector Direction)
{
    if (!Character) Character = Cast<ABBBCharacter>(GetOwner());
    if (Character && Character->IsLocallyControlled())
    {
        Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{
            {Damage}, {Bone}, {Position}, {Direction}, {nullptr}});
    }
}

void UBBBCharacterNetworkComponent::ReplicateRescue(APawn *Partner, uint64 Operation, uint64 Round, uint64 Revision,
    bool bHelping, bool bReceiving, bool bAccepted, float Elapsed, float Duration, FName Reason)
{
    if (!IsOwnerAuthority() || Revision <= ReplicatedRescueRevision)
    {
        return;
    }
    ReplicatedRescuePartner = Partner;
    ReplicatedRescueOperation = Operation;
    ReplicatedRescueRound = Round;
    ReplicatedRescueRevision = Revision;
    bReplicatedRescueHelping = bHelping;
    bReplicatedRescueReceiving = bReceiving;
    bReplicatedRescueAccepted = bAccepted;
    ReplicatedRescueStartTime = GetWorld()->GetTimeSeconds() - Elapsed;
    ReplicatedRescueDuration = Duration;
    ReplicatedRescueReason = Reason;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitRescue_Implementation(APawn *Partner, uint64 Operation,
    uint64 Round, uint64 Revision, bool bHelping, bool bReceiving, bool bAccepted, float Elapsed, float Duration, FName Reason)
{
    if (Character && IsOwnerAuthority())
    {
        Character->SubmitInput(FBBBCharacterRescueSnapshotRemoteMessagePacket{{Partner}, {Operation}, {Round},
            {Revision}, {bHelping}, {bReceiving}, {bAccepted}, {Elapsed}, {Duration}, {Reason}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Rescue()
{
    if (!Character)
    {
        Character = Cast<ABBBCharacter>(GetOwner());
    }
    if (!Character || IsOwnerAuthority() || Character->IsLocallyControlled() || ReplicatedRescueRevision == 0)
    {
        return;
    }
    const AGameStateBase *GameState = GetWorld()->GetGameState();
    const float ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
    const float Elapsed = FMath::Max(0.0f, ServerTime - ReplicatedRescueStartTime);
    Character->SubmitInput(FBBBCharacterRescueSnapshotAuthorityFactPacket{{ReplicatedRescuePartner},
        {ReplicatedRescueOperation}, {ReplicatedRescueRound}, {ReplicatedRescueRevision},
        {bReplicatedRescueHelping}, {bReplicatedRescueReceiving}, {bReplicatedRescueAccepted}, {Elapsed},
        {ReplicatedRescueDuration}, {ReplicatedRescueReason}});
}

void UBBBCharacterNetworkComponent::ServerRequestRescue_Implementation(ABBBCharacter *Target, uint64 Operation, uint64 Round)
{
    if (Character && IsOwnerAuthority() && IsValid(Target) && Target->GetWorld() == GetWorld())
    {
        Character->SubmitInput(FBBBCharacterRescueRequestRemoteMessagePacket{{Character}, {Operation}, {Round}, {Target}});
    }
}

void UBBBCharacterNetworkComponent::ClientRequestRescue_Implementation(APawn *Source, uint64 Operation, uint64 Round)
{
    if (Character)
    {
        Character->SubmitInput(FBBBCharacterRescueRequestLocalControlPacket{{Source}, {Operation}, {Round}});
    }
}

void UBBBCharacterNetworkComponent::ServerEndRescue_Implementation(ABBBCharacter *Target, uint64 Operation, uint64 Round)
{
    if (Character && IsOwnerAuthority() && IsValid(Target) && Target->GetWorld() == GetWorld())
    {
        Character->SubmitInput(FBBBCharacterRescueEndRemoteMessagePacket{{Character}, {Operation}, {Round}, {Target}});
    }
}

void UBBBCharacterNetworkComponent::ClientEndRescue_Implementation(APawn *Source, uint64 Operation, uint64 Round)
{
    if (Character)
    {
        Character->SubmitInput(FBBBCharacterRescueEndLocalControlPacket{{Source}, {Operation}, {Round}});
    }
}

void UBBBCharacterNetworkComponent::ServerReplyRescue_Implementation(ABBBCharacter *Target, uint64 Operation,
    uint64 Round, uint64 Revision, bool bActive, float Duration, FName Reason)
{
    if (Character && IsOwnerAuthority() && IsValid(Target) && Target->GetWorld() == GetWorld())
    {
        Character->SubmitInput(FBBBCharacterRescueReplyRemoteMessagePacket{{Character}, {Operation}, {Round},
            {Revision}, {bActive}, {Duration}, {Reason}, {Target}});
    }
}

void UBBBCharacterNetworkComponent::ClientReplyRescue_Implementation(APawn *Source, uint64 Operation,
    uint64 Round, uint64 Revision, bool bActive, float Duration, FName Reason)
{
    if (Character)
    {
        Character->SubmitInput(FBBBCharacterRescueReplyLocalControlPacket{{Source}, {Operation}, {Round},
            {Revision}, {bActive}, {Duration}, {Reason}});
    }
}

void UBBBCharacterNetworkComponent::ReplicateAppearance(const FBBBCharacterAppearanceSnapshot &Snapshot)
{
    if (IsOwnerAuthority() && Snapshot.IsValid() && Snapshot.Revision > ReplicatedAppearance.Revision)
    {
        ReplicatedAppearance = Snapshot;
        GetOwner()->FlushNetDormancy();
        GetOwner()->ForceNetUpdate();
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitAppearance_Implementation(FBBBCharacterAppearanceSnapshot Snapshot)
{
    if (!Character)
    {
        Character = Cast<ABBBCharacter>(GetOwner());
    }
    if (Character && IsOwnerAuthority() && !Character->IsLocallyControlled()
        && Snapshot.IsValid() && Snapshot.Revision > ReplicatedAppearance.Revision)
    {
        Character->SubmitInput(FBBBCharacterAppearanceRemoteMessagePacket{{MoveTemp(Snapshot)}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Appearance()
{
    if (!Character)
    {
        Character = Cast<ABBBCharacter>(GetOwner());
    }
    if (Character && !IsOwnerAuthority() && !Character->IsLocallyControlled() && ReplicatedAppearance.IsValid())
    {
        Character->SubmitInput(FBBBCharacterAppearanceAuthorityFactPacket{{ReplicatedAppearance}});
    }
}

void UBBBCharacterNetworkComponent::ReplicateUse(const FBBBCharacterUseFact &Fact)
{
    if (IsOwnerAuthority() && Fact.Revision > ReplicatedUse.Revision)
    {
        ReplicatedUse = Fact;
        GetOwner()->ForceNetUpdate();
    }
}
void UBBBCharacterNetworkComponent::ServerSubmitUse_Implementation(FBBBCharacterUseFact Fact)
{
    if (Character && IsOwnerAuthority() && Fact.Revision > ReplicatedUse.Revision)
    {
        Character->SubmitInput(FBBBCharacterUseRemoteMessagePacket{{Fact}});
    }
}
void UBBBCharacterNetworkComponent::OnRep_Use()
{
    if (Character && Character->IsNetworkMirror())
    {
        Character->SubmitInput(FBBBCharacterUseAuthorityFactPacket{{ReplicatedUse}});
    }
}
