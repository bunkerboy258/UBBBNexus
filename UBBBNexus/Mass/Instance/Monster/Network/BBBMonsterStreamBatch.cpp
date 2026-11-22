#include "BBBMonsterStreamBatch.h"

#include "Serialization/BitReader.h"
#include "Net/Core/Serialization/QuantizedVectorSerialization.h"

namespace
{
    bool SerializePath(FArchive& Ar, FString& Path)
    {
        constexpr int32 MaxCharacters = 512;
        TArray<ANSICHAR, TInlineAllocator<MaxCharacters>> Bytes;
        uint32 Count = 0;
        if (Ar.IsSaving())
        {
            const FTCHARToUTF8 Converted(*Path);
            if (Converted.Length() > MaxCharacters || Converted.Length() == 0)
            {
                return false;
            }
            Count = Converted.Length();
            Bytes.Append(Converted.Get(), Count);
        }
        Ar.SerializeInt(Count, MaxCharacters + 1);
        if (Count == 0 || Count > MaxCharacters)
        {
            return false;
        }
        if (Ar.IsLoading())
        {
            Bytes.SetNumUninitialized(Count);
        }
        Ar.Serialize(Bytes.GetData(), Count);
        if (Ar.IsLoading() && !Ar.IsError())
        {
            const FUTF8ToTCHAR Converted(Bytes.GetData(), Count);
            Path = FString(Converted.Length(), Converted.Get());
        }
        return !Ar.IsError();
    }

    void SerializeDamage(FArchive& Ar, FBBBMonsterDamageTotals& Value)
    {
        Ar << Value.PlayerId;
        Ar.SerializeBits(&Value.bWorldSource, 1);
        Ar << Value.Parts.Torso << Value.Parts.Head << Value.Parts.LeftArm << Value.Parts.RightArm
            << Value.Parts.LeftLeg << Value.Parts.RightLeg;
        Ar.Serialize(Value.Destruction.Regions, sizeof(Value.Destruction.Regions));
        Ar << Value.LastHitTime << Value.LastHitRegion;
        Ar.SerializeBits(&Value.bHasSourcePosition, 1);
        if (Value.bHasSourcePosition)
        {
            Ar << Value.LastSourcePosition;
        }
        Ar << Value.Stagger.ActiveAttackId << Value.Stagger.StartedAt << Value.Stagger.EndsAt << Value.Stagger.ActiveRegion;
        Ar << Value.ShockStartedAt << Value.ShockEndsAt;
        auto& Hit = Value.LastHitResult;
        Ar << Hit.Sequence;
        if (Hit.Sequence != 0)
        {
            Ar << Hit.Time << Hit.Region << Hit.Position << Hit.Direction << Hit.Normal << Hit.Impulse << Hit.Velocity;
        }
    }

    bool SerializeTraversal(FArchive& Ar, FBBBMonsterTraversalFact& Fact)
    {
        Ar.SerializeBits(&Fact.Kind, 2);
        Ar << Fact.Serial;
        if (Fact.Kind != 0)
        {
            Ar << Fact.StartedAt << Fact.Duration;
            const bool bStart = UE::Net::SerializeQuantizedVector<10>(Fact.Start, Ar);
            const bool bEnd = UE::Net::SerializeQuantizedVector<10>(Fact.End, Ar);
            const bool bLedge = UE::Net::SerializeQuantizedVector<10>(Fact.Ledge, Ar);
            return bStart && bEnd && bLedge && Fact.IsValid();
        }
        return !Ar.IsError();
    }
}

bool FBBBMonsterStreamRecord::Serialize(FArchive& Ar, UPackageMap* PackageMap)
{
    uint8 Type = static_cast<uint8>(Kind);
    Ar.SerializeBits(&Type, 3);
    Kind = static_cast<EKind>(Type);
    Ar << Action.InstanceId;
    if (Kind == EKind::Forget)
    {
        return !Ar.IsError();
    }
    if (Kind == EKind::Damage || Kind == EKind::InitialDamage)
    {
        SerializeDamage(Ar, Damage);
        return !Ar.IsError() && Damage.IsValid();
    }
    if (Kind != EKind::Motion && Kind != EKind::Birth)
    {
        return false;
    }
    Ar << Action.Revision << Action.Behavior << Action.ActionId << Action.StateEnteredTime << Action.CrawlStartedAt;
    Ar.Serialize(Action.DismembermentPresets, sizeof(Action.DismembermentPresets));
    const bool bLocation = UE::Net::SerializeQuantizedVector<10>(static_cast<FVector&>(Action.Location), Ar);
    const bool bVelocity = UE::Net::SerializeQuantizedVector<10>(static_cast<FVector&>(Action.Velocity), Ar);
    Action.Rotation.SerializeCompressedShort(Ar);
    const bool bTraversal = SerializeTraversal(Ar, Action.Traversal);
    Ar << Action.Special.Serial;
    Ar.SerializeBits(&Action.Special.Phase, 3);
    if (Action.Special.Phase != 0)
    {
        Ar << Action.Special.StartedAt << Action.Special.Duration;
        if (!UE::Net::SerializeQuantizedVector<10>(Action.Special.Goal, Ar))
        {
            return false;
        }
    }
    if (Kind == EKind::Birth)
    {
        Ar << Action.BirthSeed;
        if (!SerializePath(Ar, DefinitionPath))
        {
            return false;
        }
    }
    return bLocation && bVelocity && bTraversal && !Ar.IsError() && Action.Special.IsValid();
}

bool FBBBMonsterStreamBatch::TryAdd(FBBBMonsterStreamRecord Record, int32 BudgetBytes)
{
    if (Records.Num() >= MaxRecords || BudgetBytes > ReliableBytes || BudgetBytes * 8 <= HeaderBits)
    {
        return false;
    }
    if (Wire.GetMaxBits() == 0)
    {
        Wire = FBitWriter(BudgetBytes * 8 - HeaderBits);
        Wire.SetAllowOverflow(true);
    }
    FBitWriterMark Mark(Wire);
    if (!Record.Serialize(Wire, nullptr) || Wire.IsError() || Wire.GetNumBits() + HeaderBits > BudgetBytes * 8)
    {
        Mark.Pop(Wire);
        return false;
    }
    PayloadBits = HeaderBits + int32(Wire.GetNumBits());
    Records.Add(MoveTemp(Record));
    return true;
}

bool FBBBMonsterStreamBatch::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bSuccess)
{
    Ar << Sequence;
    uint8 Count = static_cast<uint8>(Records.Num());
    Ar.SerializeBits(&Count, 6);
    uint16 BodyBits = static_cast<uint16>(Wire.GetNumBits());
    Ar.SerializeBits(&BodyBits, 15);
    bSuccess = !Ar.IsError() && BodyBits <= ReliableBytes * 8 - HeaderBits && (Count == 0) == (BodyBits == 0);
    if (!bSuccess)
    {
        Ar.SetError();
        return true;
    }
    if (Ar.IsSaving())
    {
        if (BodyBits > 0)
        {
            Ar.SerializeBits(Wire.GetData(), BodyBits);
        }
        bSuccess = !Ar.IsError();
        return true;
    }
    PayloadBits = HeaderBits + BodyBits;
    if (BodyBits == 0)
    {
        Records.Reset();
        return true;
    }

    TArray<uint8> Data;
    Data.SetNumUninitialized((BodyBits + 7) / 8);
    Ar.SerializeBits(Data.GetData(), BodyBits);
    FBitReader Reader;
    Reader.SetData(MoveTemp(Data), BodyBits);
    Records.SetNum(Count);
    for (auto& Record : Records)
    {
        if (!Record.Serialize(Reader, PackageMap))
        {
            bSuccess = false;
            break;
        }
    }
    bSuccess = bSuccess && !Ar.IsError() && !Reader.IsError() && Reader.AtEnd();
    if (!bSuccess)
    {
        Ar.SetError();
    }
    return true;
}
