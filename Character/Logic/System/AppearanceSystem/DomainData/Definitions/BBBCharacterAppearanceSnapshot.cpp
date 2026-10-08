#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceSnapshot.h"

bool FBBBCharacterAppearanceSnapshot::IsValid() const
{
    if (Revision == 0 || Parts.IsEmpty() || Parts.Num() > 16
        || !FMath::IsFinite(Dirt) || Dirt < 0.0f || Dirt > 1.0f
        || !FMath::IsFinite(Weathering) || Weathering < 0.0f || Weathering > 1.0f)
    {
        return false;
    }
    TSet<FName> Slots;
    for (const FBBBCharacterAppearancePart &Part : Parts)
    {
        if (Part.Slot.IsNone() || Slots.Contains(Part.Slot) || Part.Colors.Num() > 8
            || Part.Patch.ContainsNaN() || Part.Patch.X < 0.0 || Part.Patch.X > 0.875
            || Part.Patch.Y < 0.0 || Part.Patch.Y > 0.875
            || (!Part.ItemId.IsNone() && !Part.InstanceId.IsValid()))
        {
            return false;
        }
        const FVector2D Cell = Part.Patch * 8.0;
        if (!FMath::IsNearlyEqual(Cell.X, FMath::RoundToDouble(Cell.X))
            || !FMath::IsNearlyEqual(Cell.Y, FMath::RoundToDouble(Cell.Y)))
        {
            return false;
        }
        Slots.Add(Part.Slot);
        for (const FLinearColor &Color : Part.Colors)
        {
            if (!FMath::IsFinite(Color.R) || !FMath::IsFinite(Color.G)
                || !FMath::IsFinite(Color.B) || !FMath::IsFinite(Color.A))
            {
                return false;
            }
        }
    }
    return true;
}
