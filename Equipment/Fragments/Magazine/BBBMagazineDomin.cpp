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

bool FBBBMagazineDomin::CanStartReload(const UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}

void FBBBMagazineDomin::CommitReload(UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return;
    }
}

float FBBBMagazineDomin::GetReloadDuration() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return 0.0f;
    }

    return 0.0f;
}

float FBBBMagazineDomin::GetMagazineRemoveNormalizedTime() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return 0.0f;
    }

    return 0.0f;
}

float FBBBMagazineDomin::GetMagazineSpawnNormalizedTime() const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return 0.0f;
    }

    return 0.0f;
}

bool FBBBMagazineDomin::SpawnMagazine(
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return false;
    }

    return false;
}

void FBBBMagazineDomin::RemoveMagazine(UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return;
    }
}

void FBBBMagazineDomin::DestroyLoadedMagazine(UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!ensureMsgf(false, TEXT("[UBBBE]Magazine domin has no fragment implementation")))
    {
        return;
    }
}
