#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"

UBBBEquipRuntimeData *FBBBEquipDomin::InitializeRuntimeData(UObject &Outer) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return nullptr;
    }

    return nullptr;
}

ABBBEquipmentPresentationActor *FBBBEquipDomin::Equip(
    UBBBEquipRuntimeData &RuntimeData,
    USkeletalMeshComponent &CharacterMesh,
    FName AttachmentSocketName) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return nullptr;
    }

    return nullptr;
}

float FBBBEquipDomin::GetEquipDuration() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return 0.0f;
    }

    return 0.0f;
}

//------------------------------------------------------------------------------

void FBBBEquipDomin::BuildEquipActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = nullptr;
    OutPresentation.PlayRate = 1.0f;

    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return;
    }
}
