#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterUseProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyAdditiveMontageAuthorityFactPacket.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterUseProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &Use = Context.RuntimeData.Interaction.InteractionUseState;
    auto &Inputs = Context.RuntimeData.Interaction.InteractionInputState;
    if (Context.bIsMirror)
    {
        if (Use.PendingFact.Revision <= Use.Fact.Revision)
        {
            return;
        }
        const bool bWasActive = Use.Fact.bActive;
        const bool bNewAction = Use.PendingFact.Serial != Use.Fact.Serial;
        Use.Fact = Use.PendingFact;
        Use.UseSerial = Use.Fact.Serial;
        const auto *Catalog = Context.Character.GetCharacterConfig().Item.ConsumableCatalog.Get();
        Use.Definition = !Use.Fact.bQuickMelee && Catalog ? Catalog->FindItem(Use.Fact.ItemId) : nullptr;
        Use.bBusy = Use.Fact.bActive;
        if (bWasActive && !Use.Fact.bActive)
        {
            const auto *Equipment = Context.Character.GetHeldEquipment();
            UAnimMontage *Return = Equipment && Equipment->GetDefinition()
                                       ? Equipment->GetDefinition()->UseReturnMontage.Get()
                                       : Context.Character.GetCharacterConfig().Animation.UseReturnMontage.Get();
            if (Return)
            {
                Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{Return});
            }
        }
        if (Use.bBusy && bNewAction && Use.Definition)
        {
            const auto *Equipment = Context.Character.GetHeldEquipment();
            TSubclassOf<UAnimInstance> LayerClass = Equipment ? Equipment->GetCharacterAnimationLayerClass() : nullptr;
            if (!LayerClass)
            {
                LayerClass = Context.Character.GetCharacterConfig().Animation.DefaultAnimationLayerClass;
            }
            const auto *Animation = LayerClass ? Cast<UBBBAnimInstance>(LayerClass.GetDefaultObject()) : nullptr;
            UAnimMontage *Montage = Use.Definition->GetUseMontage(Animation);
            if (Montage && !Montage->SlotAnimTracks.IsEmpty())
            {
                if (Montage->SlotAnimTracks[0].SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
                {
                    Context.Character.SubmitInput(FBBBUpperBodyAdditiveMontageAuthorityFactPacket{Montage});
                }
                else
                {
                    Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{Montage});
                }
            }
        }
        if (Use.bBusy && bNewAction && Use.Fact.bQuickMelee)
        {
            const auto &Config = Context.Character.GetCharacterConfig();
            const auto Class =
                Config.Item.EquipmentCatalog ? Config.Item.EquipmentCatalog->FindItem(Use.Fact.ItemId) : nullptr;
            const auto *Definition = Class ? Class.GetDefaultObject()->GetDefinition() : nullptr;
            Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{
                Definition ? Definition->QuickMeleeMontage.Get() : Config.Animation.UnarmedQuickMeleeMontage.Get()});
        }
        return;
    }
    if (Inputs.bQuickMelee && !Use.bBusy && Context.Character.IsItemUsable())
    {
        const auto *Equipment = Context.Character.GetHeldEquipment();
        const auto *Definition = Equipment ? Equipment->GetDefinition() : nullptr;
        UAnimMontage *Montage = Definition
                                    ? Definition->QuickMeleeMontage.Get()
                                    : Context.Character.GetCharacterConfig().Animation.UnarmedQuickMeleeMontage.Get();
        if (Montage)
        {
            const uint64 Revision = Use.Fact.Revision + 1;
            Use.Fact = {};
            Use.Fact.Revision = Revision;
            Use.Fact.Serial = ++Use.UseSerial;
            Use.Fact.ItemId = Equipment ? Equipment->GetEquipmentId() : FName(TEXT("Unarmed"));
            Use.Fact.bActive = true;
            Use.Fact.bQuickMelee = true;
            Use.bBusy = true;
            Use.Instance.Invalidate();
            Use.Definition = nullptr;
            Use.bMeleeContact = false;
            Use.bHasMeleePose = false;
            Use.MeleeActors.Reset();
            Use.MeleeEntities.Reset();
            Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Montage});
        }
    }
    Inputs.bQuickMelee = false;
    const auto &Owned = Context.RuntimeData.Item.ReadCharacterItemUseState();
    if (Owned.Runtime && Owned.Runtime != Use.Runtime.Get() && !Use.bBusy)
    {
        Use.Runtime = Owned.Runtime;
        Use.Definition = Owned.Runtime->GetDefinition();
        Use.Instance = Owned.Instance;
        Use.UseSerial = Owned.Serial;
        const uint64 NextRevision = Use.Fact.Revision + 1;
        Use.Fact = {};
        Use.Fact.Revision = NextRevision;
        Use.Fact.Serial = Use.UseSerial;
        Use.Fact.ItemId = Use.Definition->ItemId;
        Use.Fact.bActive = true;
        Use.bBusy = true;
    }
    auto *Runtime = Use.Runtime.Get();
    if (Runtime && Use.bBusy)
    {
        Runtime->UpdateUse();
    }
}
