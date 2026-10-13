#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/Processors/BBBGrenadeNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/DomainData/Context/BBBGrenadeNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Network/BBBConsumableNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeNetworkProcessor::Update(FBBBGrenadeNetworkUpdateContext &Context) const
{
    if (Context.Character.IsNetworkMirror() || Context.Character.GetHeldItem() != &Context.Actor)
    {
        return;
    }
    auto *Carrier = Context.Character.GetConsumableNetworkComponent();
    const auto &Use = Context.Data.Use.ReadGrenadeUseState();
    auto &Network = Context.Data.Network.NetworkState;
    if (!Carrier || (Network.UseSerial == Use.UseSerial && Network.ReleaseSerial == Use.ReleaseSerial
        && Network.bUsing == Use.bUsing))
    {
        return;
    }
    FBBBConsumableNetworkMessage Message;
    Message.ItemId = Context.Actor.GetDefinition()->ItemId;
    Message.Generation = Context.Character.GetItemGeneration();
    Message.Revision = ++Network.Revision;
    Message.UseSerial = Use.UseSerial;
    Message.ReleaseSerial = Use.ReleaseSerial;
    Message.bUsing = Use.bUsing;
    Message.bReleased = Use.bReleased;
    Message.LaunchTransform = Use.LaunchTransform;
    Carrier->Publish(Message);
    Network.UseSerial = Use.UseSerial;
    Network.ReleaseSerial = Use.ReleaseSerial;
    Network.bUsing = Use.bUsing;
}
