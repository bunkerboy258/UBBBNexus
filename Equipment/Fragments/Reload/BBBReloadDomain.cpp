#include "BBBWork/UBBBNexus/Equipment/Fragments/Reload/BBBReloadDomain.h"

#include "Animation/AnimMontage.h"

float FBBBReloadDomain::GetDurationSeconds() const
{
    return FMath::Max(DurationSeconds, 0.01f);
}

void FBBBReloadDomain::BuildPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = Montage;
    OutPresentation.PlayRate = 1.0f;

    if (!Montage)
    {
        return;
    }

    OutPresentation.PlayRate = FMath::Max(
        Montage->GetPlayLength() / GetDurationSeconds(),
        0.01f);
}
