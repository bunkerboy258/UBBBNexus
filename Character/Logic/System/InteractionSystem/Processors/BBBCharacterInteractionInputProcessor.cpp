#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionInputProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndContactLocalControlPacket.h"

void FBBBCharacterInteractionInputProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Interaction.InteractionInputState;
    auto *Equipment = Context.Character.GetHeldEquipment();
    if (State.bPrimary && Equipment)
    {
        Equipment->SubmitInput(FBBBEquipmentPrimaryLocalControlPacket<>{});
    }
    if (State.bSecondary && Equipment)
    {
        Equipment->SubmitInput(FBBBEquipmentSecondaryLocalControlPacket<>{});
    }
    State.bPrimary = false;
    State.bSecondary = false;
    for (int32 Index = 0; Index < State.BeginActionRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.BeginActionRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetHeldEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentBeginActionLocalControlPacket<>{{State.BeginActionTokens[Index]}});
        }
    }
    State.BeginActionRecipients.Reset();
    State.BeginActionTokens.Reset();

    for (int32 Index = 0; Index < State.EndActionRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.EndActionRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetHeldEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentEndActionLocalControlPacket<>{{State.EndActionTokens[Index]}});
        }
    }
    State.EndActionRecipients.Reset();
    State.EndActionTokens.Reset();

    for (int32 Index = 0; Index < State.BeginContactRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.BeginContactRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetHeldEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentBeginContactLocalControlPacket<>{{State.BeginContactTokens[Index]}});
        }
    }
    State.BeginContactRecipients.Reset();
    State.BeginContactTokens.Reset();

    for (int32 Index = 0; Index < State.EndContactRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.EndContactRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetHeldEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentEndContactLocalControlPacket<>{{State.EndContactTokens[Index]}});
        }
    }
    State.EndContactRecipients.Reset();
    State.EndContactTokens.Reset();

}
