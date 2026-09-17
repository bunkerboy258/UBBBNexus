#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(FBBBCharacterNetworkSystem &InNetworkSystem)
{
    NetworkSystem = &InNetworkSystem;
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 瞄准和移动状态只复制给远端角色所有者不重复接收自身状态
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAimState, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLocomotionState, COND_SkipOwner);
}

void UBBBCharacterNetworkComponent::ServerUploadEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    // 服务端收到装备状态后交给网络系统分发
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment packet receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentForDistribution(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::ServerUploadEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    // 服务端收到装备动作后交给网络系统分发
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment action packet receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentActionForDistribution(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::MulticastEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    // 多播装备状态进入本地网络恢复队列
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment multicast receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentForRestore(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::MulticastEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    // 多播装备动作进入本地网络恢复队列
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment action multicast receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentActionForRestore(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Aim state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveSubmittedAimState(AimState);
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(FBBBLocomotionNetworkState LocomotionState)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Locomotion state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveSubmittedLocomotionState(LocomotionState);
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    // 属性复制完成后通知网络系统恢复瞄准状态
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Replicated aim state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveReplicatedAimState(ReplicatedAimState);
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    // 属性复制完成后通知网络系统恢复移动状态
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Replicated locomotion state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveReplicatedLocomotionState(ReplicatedLocomotionState);
    }
}

void UBBBCharacterNetworkComponent::SetReplicatedAimState(const FBBBAimNetworkState &AimState)
{
    ReplicatedAimState = AimState;
}

void UBBBCharacterNetworkComponent::SetReplicatedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    ReplicatedLocomotionState = LocomotionState;
}

bool UBBBCharacterNetworkComponent::IsOwnerLocallyControlled() const
{
    const APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->IsLocallyControlled();
}

bool UBBBCharacterNetworkComponent::IsOwnerAuthority() const
{
    const APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->HasAuthority();
}

APawn *UBBBCharacterNetworkComponent::GetOwnerPawn() const
{
    // 网络组件只允许挂载在角色拥有的控制对象上
    APawn *OwnerPawn = Cast<APawn>(GetOwner());
    ensureMsgf(OwnerPawn, TEXT("[UBBBC]Network component owner is not APawn"));
    return OwnerPawn;
}
