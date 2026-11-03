#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Network/BBBMaodieTestNetworkComponent.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "Net/UnrealNetwork.h"
UBBBMaodieTestNetworkComponent::UBBBMaodieTestNetworkComponent()
{
    SetIsReplicatedByDefault(true);
}
void UBBBMaodieTestNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBBBMaodieTestNetworkComponent, Snapshot);
}
void UBBBMaodieTestNetworkComponent::Publish(const FBBBMaodieTestSnapshot &Value)
{
    Snapshot = Value;
    GetOwner()->ForceNetUpdate();
}
void UBBBMaodieTestNetworkComponent::OnRep_Snapshot()
{
    if (auto *Actor = Cast<ABBBMaodieTest>(GetOwner()))
    {
        Actor->SubmitInput(FBBBMaodieTestSnapshotAuthorityFactPacket{Snapshot});
    }
}
