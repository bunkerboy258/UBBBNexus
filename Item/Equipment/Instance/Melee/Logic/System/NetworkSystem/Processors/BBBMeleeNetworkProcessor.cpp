#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/NetworkSystem/Processors/BBBMeleeNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/NetworkSystem/DomainData/Context/BBBMeleeNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/RuntimeData/BBBMeleeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Network/BBBMeleeNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBMeleeNetworkProcessor::Update(FBBBMeleeNetworkUpdateContext &Context)
{
    auto *Network = Cast<UBBBMeleeNetworkComponent>(Context.Equipment.GetNetworkComponent());
    if (!ensureMsgf(Network, TEXT("近战缺少自身协议组件")))
    {
        return;
    }

    auto &Sent = Context.RuntimeData.Network.ObservationState;
    const auto &Action = Context.RuntimeData.Action.ReadMeleeActionState();
    const uint64 Generation = Context.Character.GetItemGeneration();
    if (Sent.Generation != Generation)
    {
        Sent.Generation = Generation;
        Sent.bPublished = false;
    }

    if (!Sent.bPublished || Sent.AttackSequence != Action.AttackSequence || Sent.bAttacking != Action.bAttacking)
    {
        bool bSent = false;
        if (Action.bAttacking)
        {
            bSent = Network->PublishAttackStart(Action.AttackSequence);
        }
        if (!Action.bAttacking)
        {
            bSent = Network->PublishAttackEnd(Action.AttackSequence);
        }
        if (!bSent)
        {
            return;
        }
        Sent.AttackSequence = Action.AttackSequence;
        Sent.bAttacking = Action.bAttacking;
    }
    Sent.bPublished = true;
}
