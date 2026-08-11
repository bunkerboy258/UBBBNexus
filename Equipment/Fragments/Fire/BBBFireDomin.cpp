#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"

UBBBFireRuntimeData *FBBBFireDomin::InitializeRuntimeData(UObject &Outer) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Fire domin has no fragment implementation")))
    {
        return nullptr;
    }

    return nullptr;
}

bool FBBBFireDomin::Fire(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBFireRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Fire domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}

void FBBBFireDomin::Present(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBFireRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Fire domin has no fragment implementation")))
    {
        return;
    }
}

void FBBBFireDomin::Update(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBFireRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Fire domin has no fragment implementation")))
    {
        return;
    }
}
