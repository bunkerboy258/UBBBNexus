#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Mirror/State/BBBAimStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Mirror/State/BBBLocomotionStatePacket.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

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

    // 本机控制角色已经生成同一份事实 只让模拟代理执行接收投递
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedAimState,
        COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedGait,
        COND_SimulatedOnly);
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(
    FBBBReplicatedAimState AimState)
{
    // 连续状态允许覆盖 但非法向量不能污染角色输入
    if (!ensureMsgf(
        Character
            && IsOwnerAuthority()
            && !FVector(AimState.AimTargetWorld).ContainsNaN()
            && FMath::IsFinite(AimState.AimAlpha)
            && AimState.AimAlpha >= 0.0f
            && AimState.AimAlpha <= 1.0f,
        TEXT("瞄准状态网络投递被拒绝")))
    {
        return;
    }

    SubmitAimStateInput(AimState);
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(
    const EBBBCharacterGait Gait)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(), TEXT("步态网络投递被拒绝")))
    {
        return;
    }

    SubmitLocomotionStateInput(Gait);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    SubmitAimStateInput(ReplicatedAimState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedGait()
{
    SubmitLocomotionStateInput(ReplicatedGait);
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

void UBBBCharacterNetworkComponent::ReplicateLocomotionState(
    const EBBBCharacterGait Gait)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    ReplicatedGait = Gait;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::SubmitAimStateInput(
    const FBBBReplicatedAimState &AimState)
{
    if (!Character)
    {
        return;
    }

    FBBBAimStatePacket Packet;
    Packet.bIsAiming = AimState.bIsAiming;
    Packet.AimAlpha = AimState.AimAlpha;
    Packet.AimTargetWorld = AimState.AimTargetWorld;
    Character->SubmitInput(MoveTemp(Packet));
}

void UBBBCharacterNetworkComponent::SubmitLocomotionStateInput(
    const EBBBCharacterGait Gait)
{
    if (Character)
    {
        Character->SubmitInput(FBBBLocomotionStatePacket{Gait});
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
