#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/Processors/BBBGrenadeNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/DomainData/Context/BBBGrenadeNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Network/BBBGrenadeNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeNetworkProcessor::Update(FBBBGrenadeNetworkUpdateContext &Context) const
{
    if (Context.Character.IsNetworkMirror() || Context.Character.GetHeldItem() != &Context.Actor)
    {
        return;
    }
    auto *Protocol = Cast<UBBBGrenadeNetworkComponent>(Context.Actor.GetNetworkComponent());
    const auto &Use = Context.Data.Use.ReadGrenadeUseState();
    auto &Network = Context.Data.Network.NetworkObservationState;
    if (!Protocol || (Network.UseSerial == Use.UseSerial && Network.ReleaseSerial == Use.ReleaseSerial
        && Network.bUsing == Use.bUsing))
    {
        return;
    }
    FBBBGrenadeNetworkMessage Message;
    Message.Revision = ++Network.Revision;
    Message.UseSerial = Use.UseSerial;
    Message.ReleaseSerial = Use.ReleaseSerial;
    Message.bUsing = Use.bUsing;
    Message.bReleased = Use.bReleased;
    Message.LaunchTransform = Use.LaunchTransform;
    if (!Protocol->PublishState(Message))
    {
        return;
    }
    Network.UseSerial = Use.UseSerial;
    Network.ReleaseSerial = Use.ReleaseSerial;
    Network.bUsing = Use.bUsing;
}

void FBBBGrenadeNetworkProcessor::Clear(FBBBGrenadeRuntimeData &Data)
{
    auto &State = Data.Network.NetworkObservationState;
    State.UseSerial = INDEX_NONE;
    State.ReleaseSerial = INDEX_NONE;
    State.bUsing = false;
    State.Revision = 0;
}
