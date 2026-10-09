#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/NetworkSystem/Processors/BBBPistolNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/Context/BBBPistolUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Network/BBBPistolNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBPistolNetworkProcessor::Update(FBBBPistolUpdateContext &Context)
{
    auto *Network = Cast<UBBBPistolNetworkComponent>(Context.Equipment.GetNetworkComponent());
    if (!ensureMsgf(Network, TEXT("手枪缺少自身协议组件")))
    {
        return;
    }

    auto &Sent = Context.RuntimeData.Network.ObservationState;
    const auto &Action = Context.RuntimeData.Action.ReadPistolActionState();
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
