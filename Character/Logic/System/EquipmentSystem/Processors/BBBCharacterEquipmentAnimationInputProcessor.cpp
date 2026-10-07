#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentAnimationInputProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndContactLocalControlPacket.h"

void FBBBCharacterEquipmentAnimationInputProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Equipment.EquipmentAnimationInputState;
    for (int32 Index = 0; Index < State.BeginActionRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.BeginActionRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetActiveEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentBeginActionLocalControlPacket{{State.BeginActionTokens[Index]}});
        }
    }
    State.BeginActionRecipients.Reset();
    State.BeginActionTokens.Reset();

    for (int32 Index = 0; Index < State.EndActionRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.EndActionRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetActiveEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentEndActionLocalControlPacket{{State.EndActionTokens[Index]}});
        }
    }
    State.EndActionRecipients.Reset();
    State.EndActionTokens.Reset();

    for (int32 Index = 0; Index < State.BeginContactRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.BeginContactRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetActiveEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentBeginContactLocalControlPacket{{State.BeginContactTokens[Index]}});
        }
    }
    State.BeginContactRecipients.Reset();
    State.BeginContactTokens.Reset();

    for (int32 Index = 0; Index < State.EndContactRecipients.Num(); ++Index)
    {
        ABBBEquipment *Recipient = State.EndContactRecipients[Index].Get();
        if (Recipient && Recipient == Context.Character.GetActiveEquipment())
        {
            Recipient->SubmitInput(FBBBEquipmentEndContactLocalControlPacket{{State.EndContactTokens[Index]}});
        }
    }
    State.EndContactRecipients.Reset();
    State.EndContactTokens.Reset();

}
