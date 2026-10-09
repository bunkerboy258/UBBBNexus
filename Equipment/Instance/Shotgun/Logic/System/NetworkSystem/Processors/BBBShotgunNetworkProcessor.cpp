#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/Processors/BBBShotgunNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Network/BBBShotgunNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBShotgunNetworkProcessor::Update(FBBBShotgunUpdateContext &Context)
{
    auto *Network = Cast<UBBBShotgunNetworkComponent>(Context.Equipment.GetNetworkComponent());
    if (!ensureMsgf(Network, TEXT("霰弹枪缺少自身协议组件")))
    {
        return;
    }

    auto &Sent = Context.RuntimeData.Network.ObservationState;
    const auto &Action = Context.RuntimeData.Action.ReadShotgunActionState();
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
