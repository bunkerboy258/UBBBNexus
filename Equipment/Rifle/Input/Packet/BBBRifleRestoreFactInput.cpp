#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleRestoreFactInput.h"

#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
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
    Context.RuntimeData.LoadedAmmo = FMath::Clamp(
        Fact.LoadedAmmo,
        0,
        Context.RuntimeData.AmmoCapacity);

    switch (Fact.Type)
    {
        case EBBBEquipmentActionType::Equip:
            Context.SubmitCharacterMontage(
                Context.Definition.CharacterEquipMontage,
                Fact.Sequence,
                false);
            return;

        case EBBBEquipmentActionType::Fire:
            Context.RuntimeData.FireSequence = Fact.Sequence;
            Context.RuntimeData.LastFireTimeSeconds = Context.GetWorld()
                ? Context.GetWorld()->GetTimeSeconds()
                : Context.RuntimeData.LastFireTimeSeconds;
            Context.PlayFireSound();
            Context.PlayEquipmentMontage(Context.Definition.EquipmentFireMontage);
            return;

        case EBBBEquipmentActionType::ReloadStarted:
            Context.RuntimeData.bIsReloading = true;
            Context.RuntimeData.bMagazineDetached = false;
            Context.RuntimeData.ReloadSequence = Fact.Sequence;
            Context.SubmitCharacterMontage(
                Context.Definition.CharacterReloadMontage,
                Fact.Sequence,
                true);
            Context.PlayEquipmentMontage(Context.Definition.EquipmentReloadMontage);
            return;

        case EBBBEquipmentActionType::MagazineDetached:
            Context.RuntimeData.bMagazineDetached = true;
            return;

        case EBBBEquipmentActionType::MagazineLoaded:
            Context.RuntimeData.bIsReloading = false;
            Context.RuntimeData.bMagazineDetached = false;
            return;

        case EBBBEquipmentActionType::ReloadCancelled:
            Context.RuntimeData.bIsReloading = false;
            Context.RuntimeData.bMagazineDetached = false;
            return;

        case EBBBEquipmentActionType::None:
            ensureMsgf(false, TEXT("步枪收到空装备事实"));
            return;
    }
}
