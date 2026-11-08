#include "BBBMonsterDirectorNetworkComponent.h"
#include "Net/UnrealNetwork.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterDirectorPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Director/FBBBMonsterDirectorStateAuthorityFactPacket.h"

UBBBMonsterDirectorNetworkComponent::UBBBMonsterDirectorNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBMonsterDirectorNetworkComponent::Publish(const FBBBMonsterDirectorReplicationState& Current)
{
    if (GetOwner()->HasAuthority() && Current.Revision != State.Revision)
    {
        State = Current;
    }
}

void UBBBMonsterDirectorNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBBBMonsterDirectorNetworkComponent, State);
}

void UBBBMonsterDirectorNetworkComponent::OnRep_State()
{
    auto* Owner = Cast<ABBBMonsterDirectorPresentationActor>(GetOwner());
    if (Owner && !Owner->HasAuthority())
    {
        FBBBMonsterDirectorStateAuthorityFactPacket Packet;
        Packet.State = State;
        Packet.Definition = Owner->Definition.Get();
        Packet.Presentation = Owner;
        Owner->SubmitInput(MoveTemp(Packet));
    }
}
