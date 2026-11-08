#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDestructionContribution.generated.h"

/** 一个玩家对八处躯干创口的累计加深值 同一玩家逐项取大 */
USTRUCT()
struct FBBBMonsterDestructionContribution final
{
    GENERATED_BODY()

    /** 八个字节表达当前结果 255 表示完全毁伤 */
    UPROPERTY()
    uint8 Regions[8] = {};

    /** @param Index 创口下标 @param Amount 已成立的加深量 @return 无 */
    void Add(int32 Index, float Amount)
    {
        if (Index >= 0 && Index < 8 && FMath::IsFinite(Amount) && Amount > 0)
        {
            Regions[Index] = static_cast<uint8>(FMath::Min(255,
                static_cast<int32>(Regions[Index]) + FMath::Max(1, FMath::RoundToInt(FMath::Min(Amount, 1.0f) * 255))));
        }
    }

    /** @param Other 同一玩家的当前结果 @return 无 */
    void MergeMax(const FBBBMonsterDestructionContribution& Other)
    {
        for (int32 Index = 0; Index < 8; ++Index)
        {
            Regions[Index] = FMath::Max(Regions[Index], Other.Regions[Index]);
        }
    }

    /** @param Other 既有结果 @return 是否包含任一区增加 */
    bool Exceeds(const FBBBMonsterDestructionContribution& Other) const
    {
        for (int32 Index = 0; Index < 8; ++Index)
        {
            if (Regions[Index] > Other.Regions[Index])
            {
                return true;
            }
        }
        return false;
    }

    /** @param Other 对照结果 @return 八区结果是否一致 */
    bool operator==(const FBBBMonsterDestructionContribution& Other) const
    {
        return FMemory::Memcmp(Regions, Other.Regions, sizeof(Regions)) == 0;
    }
};
