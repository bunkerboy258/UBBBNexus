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

bool FBBBEquipDomin::IsLeftHandIKEnabled() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Equip domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}
