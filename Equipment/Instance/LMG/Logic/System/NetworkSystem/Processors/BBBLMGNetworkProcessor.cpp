#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/NetworkSystem/Processors/BBBLMGNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/Context/BBBLMGUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Network/BBBLMGNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBLMGNetworkProcessor::Update(FBBBLMGUpdateContext &Context)
{
    auto *Network = Cast<UBBBLMGNetworkComponent>(Context.Equipment.GetNetworkComponent());
    if (!ensureMsgf(Network, TEXT("轻机枪缺少自身协议组件")))
    {
        return;
    }

    auto &Sent = Context.RuntimeData.Network.ObservationState;
    const auto &Action = Context.RuntimeData.Action.ReadLMGActionState();
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
