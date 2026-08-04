#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"

UBBBMagazineRuntimeData *FBBBMagazineDomin::InitializeRuntimeData(UObject &Outer) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return nullptr;
    }

    return nullptr;
}

bool FBBBMagazineDomin::CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}

void FBBBMagazineDomin::ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return;
    }
}

bool FBBBMagazineDomin::Reload(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}

void FBBBMagazineDomin::PresentReload(FBBBCharacterExternalAPI &CharacterAPI) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return;
    }
}

void FBBBMagazineDomin::Update(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return;
    }
}
