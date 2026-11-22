#pragma once

#include "BBBMonsterStreamRecord.h"
#include "Serialization/BitWriter.h"
#include "BBBMonsterStreamBatch.generated.h"

/** 按实际编码位数装配的独立小怪传输批次 */
USTRUCT()
struct FBBBMonsterStreamBatch final
{
    GENERATED_BODY()

    static constexpr int32 MotionBytes = 1000;
    static constexpr int32 ReliableBytes = 3500;
    static constexpr int32 MaxRecords = 63;
    static constexpr int32 HeaderBits = 32 + 6 + 15;

    TArray<FBBBMonsterStreamRecord> Records;
    uint32 Sequence = 0;
    int32 PayloadBits = HeaderBits;

    bool TryAdd(FBBBMonsterStreamRecord Record, int32 BudgetBytes);
    bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bSuccess);
    bool Identical(const FBBBMonsterStreamBatch* Other, uint32 PortFlags) const
    {
        return Other ? Sequence == Other->Sequence : Sequence == 0;
    }

private:
    FBitWriter Wire;
};

template<>
struct TStructOpsTypeTraits<FBBBMonsterStreamBatch> : TStructOpsTypeTraitsBase2<FBBBMonsterStreamBatch>
{
    enum { WithNetSerializer = true, WithIdentical = true };
};
