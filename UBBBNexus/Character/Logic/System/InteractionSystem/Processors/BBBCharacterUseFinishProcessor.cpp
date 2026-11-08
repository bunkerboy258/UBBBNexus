#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterUseFinishProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
void FBBBCharacterUseFinishProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &Use = Context.RuntimeData.Interaction.InteractionUseState;
    auto &Inputs = Context.RuntimeData.Interaction.InteractionInputState;
    const bool bNotified = Inputs.FinishTokens.Contains(Use.UseSerial);
    const bool bInterrupted = !Context.Character.IsItemUsable();
    if (!Context.bIsMirror && Use.bBusy && (bNotified || bInterrupted))
    {
        if (auto *Runtime = Use.Runtime.Get())
        {
            Runtime->SubmitInput(FBBBConsumableFinishLocalControlPacket<>{});
            Runtime->UpdateUse();
        }
        Use.bMeleeContact = false;
        Use.bHasMeleePose = false;
        Use.bBusy = false;
        Use.Fact.bActive = false;
        ++Use.Fact.Revision;
        Use.Runtime.Reset();
        const auto *Equipment = Context.Character.GetHeldEquipment();
        UAnimMontage *Return = Equipment && Equipment->GetDefinition()
                                   ? Equipment->GetDefinition()->UseReturnMontage.Get()
                                   : Context.Character.GetCharacterConfig().Animation.UseReturnMontage.Get();
        if (Return)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Return});
        }
    }
    Inputs.FinishRecipients.Reset();
    Inputs.FinishGenerations.Reset();
    Inputs.FinishTokens.Reset();
}
