#include "BBBWork/UBBBNexus/Item/WorldDrop/Network/BBBWorldDropNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/AuthorityFact/Snapshot/FBBBWorldDropSnapshotAuthorityFactPacket.h"
#include "Net/UnrealNetwork.h"
UBBBWorldDropNetworkComponent::UBBBWorldDropNetworkComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = false;
}
void UBBBWorldDropNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBBBWorldDropNetworkComponent, ReplicatedSnapshot);
}
void UBBBWorldDropNetworkComponent::Publish(const FBBBWorldDropSnapshot &Snapshot)
{
    if (GetOwner()->HasAuthority() && Snapshot.Revision > ReplicatedSnapshot.Revision)
    {
        ReplicatedSnapshot = Snapshot;
        GetOwner()->ForceNetUpdate();
    }
}
void UBBBWorldDropNetworkComponent::OnRep_Snapshot()
{
    if (auto *Drop = Cast<ABBBWorldDropItem>(GetOwner()))
    {
        Drop->SubmitInput(FBBBWorldDropSnapshotAuthorityFactPacket{{ReplicatedSnapshot}});
    }
}
