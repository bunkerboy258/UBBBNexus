#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/NetworkSystem/Processors/BBBSniperNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/Context/BBBSniperUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Network/BBBSniperNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBSniperNetworkProcessor::Update(FBBBSniperUpdateContext &Context)
{
    auto *Network = Cast<UBBBSniperNetworkComponent>(Context.Equipment.GetNetworkComponent());
    if (!ensureMsgf(Network, TEXT("狙击枪缺少自身协议组件")))
    {
        return;
    }

    auto &Sent = Context.RuntimeData.Network.ObservationState;
    const auto &Action = Context.RuntimeData.Action.ReadSniperActionState();
    const uint64 Generation = Context.Character.GetEquipmentGeneration();
    if (Sent.Generation != Generation)
    {
        Sent.Generation = Generation;
        Sent.bPublished = false;
    }

    if (!Sent.bPublished || Sent.FireSequence != Action.FireSequence)
    {
        if (!Network->PublishFire(Action.FireSequence))
        {
            return;
        }
        Sent.FireSequence = Action.FireSequence;
    }

    if (!Sent.bPublished || Sent.ReloadSequence != Action.ReloadSequence || Sent.bReloading != Action.bIsReloading)
    {
        bool bSent = false;
        if (Action.bIsReloading)
        {
            bSent = Network->PublishReloadStart(Action.ReloadSequence);
        }
        if (!Action.bIsReloading)
        {
            bSent = Network->PublishReloadEnd(Action.ReloadSequence, Action.bReloadCompletedThisFrame);
        }
        if (!bSent)
        {
            return;
        }
        Sent.ReloadSequence = Action.ReloadSequence;
        Sent.bReloading = Action.bIsReloading;
    }
    Sent.bPublished = true;
}
