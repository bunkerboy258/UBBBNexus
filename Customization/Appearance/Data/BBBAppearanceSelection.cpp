#include "BBBWork/UBBBNexus/Customization/Appearance/Data/BBBAppearanceSelection.h"

bool FBBBAppearanceSelection::IsValid() const
{
    //这里只约束记录大小和数值范围 不在网络接收时重新决定部件搭配
    if (Parts.IsEmpty() || Parts.Num() > 16
        || !FMath::IsFinite(Dirt) || Dirt < 0.0f || Dirt > 1.0f
        || !FMath::IsFinite(Weathering) || Weathering < 0.0f || Weathering > 1.0f)
    {
        return false;
    }

    TSet<FName> Slots;
    for (const FBBBAppearancePart &Part : Parts)
    {
        if (Part.Slot.IsNone() || Slots.Contains(Part.Slot) || Part.Colors.Num() > 8
            || Part.Patch.ContainsNaN()
            || Part.Patch.X < 0.0 || Part.Patch.X > 0.875
            || Part.Patch.Y < 0.0 || Part.Patch.Y > 0.875)
        {
            return false;
        }

        // 图集只接受完整格子的起点 防止存档或网络输入采样到相邻图案
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
