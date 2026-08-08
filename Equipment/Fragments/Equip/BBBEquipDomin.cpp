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
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return nullptr;
    }

    return nullptr;
}

void FBBBEquipDomin::Update(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBEquipRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return;
    }
}

FName FBBBEquipDomin::GetAimSourceSocketName() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return NAME_None;
    }

    return NAME_None;
}

FName FBBBEquipDomin::GetLeftHandGripSocketName() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return NAME_None;
    }

    return NAME_None;
}

const FTransform &FBBBEquipDomin::GetLeftHandGripSocketLocalOffset() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return FTransform::Identity;
    }

    return FTransform::Identity;
}

bool FBBBEquipDomin::IsLeftHandIKEnabled() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}

UAnimSequence *FBBBEquipDomin::GetEquippedUpperBodyAnimation() const
{
    return nullptr;
}
