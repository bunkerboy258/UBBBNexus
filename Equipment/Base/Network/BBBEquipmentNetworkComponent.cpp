#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Shared/Action/BBBEquipmentSelectionPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkPayload.h"
#include "Net/UnrealNetwork.h"

UBBBEquipmentNetworkComponent::UBBBEquipmentNetworkComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UBBBEquipmentNetworkComponent::BeginPlay()
{
    Super::BeginPlay();
    if (AActor *Owner = GetOwner())
    {
        AddTickPrerequisiteActor(Owner);
    }
}

void UBBBEquipmentNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBEquipmentNetworkComponent, ReplicatedState, COND_SimulatedOnly);
}

void UBBBEquipmentNetworkComponent::PublishState(ABBBEquipment *Equipment, const TArray<uint8> &Data)
{
    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || Character->RuntimeData.External.ReadNetworkIdentityState().bIsMirror
        || Character->GetActiveEquipment() != Equipment)
    {
        return;
    }

    if (!ensureMsgf(Data.Num() <= 128, TEXT("装备当前状态超过传输容量")))
    {
        return;
    }

    const TWeakObjectPtr<ABBBEquipment> Current(Equipment);
    const bool bChangedEquipment = PublishedEquipment != Current || ReplicatedState.Generation == 0;
    if (!bChangedEquipment && ReplicatedState.Data == Data)
    {
        return;
    }

    if (bChangedEquipment)
    {
        ++ReplicatedState.Generation;
        ReplicatedState.Revision = 0;
        PublishedEquipment = Current;
    }

    ReplicatedState.EquipmentId = Equipment ? Equipment->GetEquipmentId() : NAME_None;
    ReplicatedState.Data = Data;
    ++ReplicatedState.Revision;
    if (Character->RuntimeData.External.ReadNetworkIdentityState().bHasAuthority)
    {
        Character->ForceNetUpdate();
        return;
    }

    ServerSubmitState(ReplicatedState);
}

void UBBBEquipmentNetworkComponent::ServerSubmitState_Implementation(FBBBEquipmentNetworkState State)
{
    const ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    if (!ensureMsgf(Character && Character->HasAuthority()
        && State.Generation > 0 && State.Revision > 0 && State.Data.Num() <= 128
        && (!State.EquipmentId.IsNone() || State.Data.IsEmpty()),
        TEXT("装备状态传输边界无效")))
    {
        return;
    }

    if (State.Generation < ReplicatedState.Generation
        || (State.Generation == ReplicatedState.Generation && State.Revision <= ReplicatedState.Revision))
    {
        UE_LOG(LogTemp, Verbose, TEXT("丢弃过期装备状态 Generation=%llu Revision=%llu"), State.Generation, State.Revision);
        return;
    }

    // 房主只保留并分发结果 不重新执行发送者的动作
    ReplicatedState = MoveTemp(State);
    GetOwner()->ForceNetUpdate();
}

void UBBBEquipmentNetworkComponent::TickComponent(
    const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    ABBBEquipment *Equipment = Character->GetActiveEquipment();
    if (!Character->RuntimeData.External.ReadNetworkIdentityState().bIsMirror)
    {
        if (!IsValid(Equipment))
        {
            PublishState(nullptr, {});
        }
        return;
    }

    if (ReplicatedState.Generation == 0)
    {
        return;
    }

    if (RequestedGeneration != ReplicatedState.Generation)
    {
        PreviousEquipment = Equipment;
        if (!Character->SubmitInput(FBBBEquipmentSelectionPacket{ReplicatedState.EquipmentId}))
        {
            return;
        }

        RequestedGeneration = ReplicatedState.Generation;
        AppliedRevision = 0;
        return;
    }

    if (AppliedRevision == ReplicatedState.Revision)
    {
        return;
    }

    if (ReplicatedState.EquipmentId.IsNone())
    {
        if (!IsValid(Equipment))
        {
            AppliedRevision = ReplicatedState.Revision;
        }
        return;
    }

    // 即使定义相同也等待新的持有实例 防止旧枪接收切回后的状态
    if (!IsValid(Equipment) || Equipment == PreviousEquipment.Get()
        || Equipment->GetEquipmentId() != ReplicatedState.EquipmentId)
    {
        return;
    }

    if (!ensureMsgf(Equipment->SubmitInput(FBBBEquipmentNetworkPayload{ReplicatedState.Data}),
        TEXT("装备拒绝当前网络状态 %s Generation=%llu Revision=%llu"),
        *ReplicatedState.EquipmentId.ToString(), ReplicatedState.Generation, ReplicatedState.Revision))
    {
        return;
    }

    AppliedRevision = ReplicatedState.Revision;
}
