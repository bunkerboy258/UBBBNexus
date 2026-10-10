#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/NetworkSystem/Processors/BBBMinigunNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/Context/BBBMinigunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Network/BBBMinigunNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBMinigunNetworkProcessor::Update(FBBBMinigunUpdateContext &Context)
{
    auto *Network = Cast<UBBBMinigunNetworkComponent>(Context.Equipment.GetNetworkComponent());
    if (!ensureMsgf(Network, TEXT("转管机枪缺少自身协议组件")))
    {
        return;
    }

    auto &Sent = Context.RuntimeData.Network.ObservationState;
    const auto &Action = Context.RuntimeData.Action.ReadMinigunActionState();
    const uint64 Generation = Context.Character.GetEquipmentGeneration();
    if (Sent.Generation != Generation)
    {
        Sent.Generation = Generation;
        Sent.bPublished = false;
    }

    if (!Sent.bPublished || Sent.bSpinning != Action.bIsSpinning)
    {
        if (!Network->PublishSpin(Action.bIsSpinning))
        {
            return;
        }
        Sent.bSpinning = Action.bIsSpinning;
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
