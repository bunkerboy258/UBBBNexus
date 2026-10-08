#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalEndAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalStartRemoteMessagePacket.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Aim/FBBBAimStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBRunStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBAccelerationAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Locomotion/FBBBAccelerationRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Equipment/FBBBEquipmentSelectionRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Equipment/FBBBEquipmentSelectionAuthorityFactPacket.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterLifeAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterLifeRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterDamageDeliveryRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Equipment/FBBBCharacterEquipmentUseAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Equipment/FBBBCharacterEquipmentUseRemoteMessagePacket.h"

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
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAcceleration, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedMovementInput, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAccelerationRevision, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLifePhase, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHealth, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLifeRevision, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitSerial, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitBone, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitPosition, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedHitDirection, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentId, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentGeneration, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, bReplicatedEquipmentUsable, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentUseRevision, COND_SimulatedOnly);

    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedTraversalId, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedTraversalAction, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedTraversalContact, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedTraversalEnd, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedTraversalStartTime, COND_SimulatedOnly);

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

void UBBBCharacterNetworkComponent::ReplicateTraversal(uint32 Id, EBBBTraversalAction Action,
    const FTransform &Contact, const FTransform &End, const float Position)
{
    if (!IsOwnerAuthority() || Id == 0)
    {
        return;
    }

    if (Action != EBBBTraversalAction::None && Id != ReplicatedTraversalId)
    {
        ReplicatedTraversalStartTime = GetWorld()->GetTimeSeconds() - Position;
    }
    ReplicatedTraversalId = Id;
    ReplicatedTraversalAction = Action;
    ReplicatedTraversalContact = Contact;
    ReplicatedTraversalEnd = End;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitTraversal_Implementation(uint32 Id,
    EBBBTraversalAction Action, FTransform Contact, FTransform End, const float Position)
{
    if (!Character || !IsOwnerAuthority() || Id == 0 || !Contact.IsValid() || !End.IsValid()
        || Action > EBBBTraversalAction::ClimbHigh || !FMath::IsFinite(Position) || Position < 0.0f)
    {
        return;
    }
    if (Action == EBBBTraversalAction::None)
    {
        Character->SubmitInput(FBBBTraversalEndRemoteMessagePacket{{Id}});
        return;
    }
    Character->SubmitInput(FBBBTraversalStartRemoteMessagePacket{{Id}, {Action}, {Contact}, {End}, {Position}});
}

void UBBBCharacterNetworkComponent::OnRep_Traversal()
{
    if (!Character)
    {
        Character = Cast<ABBBCharacter>(GetOwner());
    }
    if (!Character || IsOwnerAuthority() || Character->IsLocallyControlled() || ReplicatedTraversalId == 0)
    {
        return;
    }
    if (ReplicatedTraversalAction == EBBBTraversalAction::None)
    {
        Character->SubmitInput(FBBBTraversalEndAuthorityFactPacket{{ReplicatedTraversalId}});
        return;
    }

    const AGameStateBase *GameState = GetWorld()->GetGameState();
    const float ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
    const float Position = FMath::Max(0.0f, ServerTime - ReplicatedTraversalStartTime);
    Character->SubmitInput(FBBBTraversalStartAuthorityFactPacket{
        {ReplicatedTraversalId}, {ReplicatedTraversalAction}, {ReplicatedTraversalContact}, {ReplicatedTraversalEnd}, {Position}});
}

void UBBBCharacterNetworkComponent::ReplicateEquipment(const FName EquipmentId, const uint64 Generation,
    const bool bUsable, const uint64 UseRevision)
{
    if (!IsOwnerAuthority() || Generation == 0 || Generation < ReplicatedEquipmentGeneration)
    {
        return;
    }

    ReplicatedEquipmentId = EquipmentId;
    ReplicatedEquipmentGeneration = Generation;
    bReplicatedEquipmentUsable = bUsable;
    ReplicatedEquipmentUseRevision = UseRevision;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipment_Implementation(const FName EquipmentId, const uint64 Generation,
    const bool bUsable, const uint64 UseRevision)
{
    if (Character && IsOwnerAuthority() && Generation > 0 && UseRevision > ReplicatedEquipmentUseRevision)
    {
        Character->SubmitInput(FBBBEquipmentSelectionRemoteMessagePacket{{EquipmentId}, {Generation}});
        Character->SubmitInput(FBBBCharacterEquipmentUseRemoteMessagePacket{{Generation}, {UseRevision}, {bUsable}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Equipment()
{
    if (Character && Character->IsNetworkMirror() && ReplicatedEquipmentGeneration > 0)
    {
        Character->SubmitInput(FBBBEquipmentSelectionAuthorityFactPacket{{ReplicatedEquipmentId}, {ReplicatedEquipmentGeneration}});
        if (ReplicatedEquipmentUseRevision > 0)
        {
            Character->SubmitInput(FBBBCharacterEquipmentUseAuthorityFactPacket{
                {ReplicatedEquipmentGeneration}, {ReplicatedEquipmentUseRevision}, {bReplicatedEquipmentUsable}});
        }
    }
}

void UBBBCharacterNetworkComponent::ReplicateLife(EBBBCharacterLifePhase Phase, float Health,
    uint64 Revision, uint64 HitSerial, FName Bone, FVector Position, FVector Direction)
{
    ReplicatedLifePhase = Phase;
    ReplicatedHealth = Health;
    ReplicatedLifeRevision = Revision;
    ReplicatedHitSerial = HitSerial;
    ReplicatedHitBone = Bone;
    ReplicatedHitPosition = Position;
    ReplicatedHitDirection = Direction;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitLife_Implementation(EBBBCharacterLifePhase Phase, float Health,
    uint64 Revision, uint64 HitSerial, FName Bone, FVector Position, FVector Direction)
{
    if (Character)
    {
        Character->SubmitInput(FBBBCharacterLifeRemoteMessagePacket{
            {Phase}, {Health}, {Revision}, {HitSerial}, {Bone}, {Position}, {Direction}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Life()
{
    if (!Character) Character = Cast<ABBBCharacter>(GetOwner());
    if (Character && ReplicatedLifeRevision > 0)
    {
        Character->SubmitInput(FBBBCharacterLifeAuthorityFactPacket{
            {ReplicatedLifePhase}, {ReplicatedHealth}, {ReplicatedLifeRevision},
            {ReplicatedHitSerial}, {ReplicatedHitBone}, {ReplicatedHitPosition}, {ReplicatedHitDirection}});
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
