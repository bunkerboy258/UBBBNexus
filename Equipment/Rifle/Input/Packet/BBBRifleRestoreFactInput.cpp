#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleRestoreFactInput.h"

#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Processors/BBBRiflePresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"
#include "Engine/World.h"

bool FBBBRifleRestoreFactInput::IsValid() const
{
    return Fact.Type != EBBBEquipmentActionType::None
        && !Fact.EquipmentId.IsNone()
        && Fact.Sequence != INDEX_NONE;
}

bool FBBBRifleRestoreFactInput::CanApply(const FBBBRifleInputContext &Context) const
{
    return Context.Equipment.IsMirror()
        && Fact.EquipmentId == Context.Equipment.GetEquipmentId();
}

void FBBBRifleRestoreFactInput::Apply(FBBBRifleInputContext &Context) const
{
    auto &State = Context.RuntimeData.Rifle.Action;
    State.LoadedAmmo = FMath::Clamp(
        Fact.LoadedAmmo,
        0,
        State.AmmoCapacity);

    switch (Fact.Type)
    {
        case EBBBEquipmentActionType::Equip:
            FBBBRiflePresentationProcessor::SubmitCharacterMontage(
                Context,
                Context.Definition.CharacterEquipMontage);
            return;

        case EBBBEquipmentActionType::Fire:
            State.FireSequence = Fact.Sequence;
            State.LastFireTimeSeconds = Context.World
                ? Context.World->GetTimeSeconds()
                : State.LastFireTimeSeconds;
            FBBBRiflePresentationProcessor::PlayFireSound(Context);
            FBBBRiflePresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentFireMontage);
            return;

        case EBBBEquipmentActionType::ReloadStarted:
            State.bIsReloading = true;
            State.bMagazineDetached = false;
            State.ReloadSequence = Fact.Sequence;
            FBBBRiflePresentationProcessor::SubmitCharacterMontage(
                Context,
                Context.Definition.CharacterReloadMontage);
            FBBBRiflePresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
            return;

        case EBBBEquipmentActionType::MagazineDetached:
            State.bMagazineDetached = true;
            return;

        case EBBBEquipmentActionType::MagazineLoaded:
            State.bIsReloading = false;
            State.bMagazineDetached = false;
            return;

        case EBBBEquipmentActionType::ReloadCancelled:
            State.bIsReloading = false;
            State.bMagazineDetached = false;
            return;

        case EBBBEquipmentActionType::None:
            ensureMsgf(false, TEXT("步枪收到空装备事实"));
            return;
    }
}
