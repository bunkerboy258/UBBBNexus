#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Serialization/BitReader.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterStreamBatch.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterStreamBudgetTest, "UBBB.Mass.Zombie.StreamBudget",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterStreamBudgetTest::RunTest(const FString&)
{
    using EKind = FBBBMonsterStreamRecord::EKind;
    FBBBMonsterStreamRecord Motion;
    Motion.Action.InstanceId = FGuid(10, 20, 30, 40);
    Motion.Action.Revision = 83;
    Motion.Action.Location = FVector(37421.7, -48312.4, 10182.1);
    Motion.Action.Traversal.Kind = 2;
    Motion.Action.Traversal.Duration = 1.7f;
    Motion.Action.Traversal.End = FVector(37200.0, -48000.3, 10210.2);
    Motion.Action.Special.Phase = 2;
    Motion.Action.Special.RushDistanceCm = 1243.75f;
    Motion.Action.Special.Goal = FVector(36001.3, -47351.8, 194.0);

    FBBBMonsterStreamRecord Birth = Motion;
    Birth.Kind = EKind::Birth;
    Birth.Action.Special.Phase = 7;
    Birth.Action.Special.RushDistanceCm = 0;
    Birth.DefinitionPath = FString::ChrN(512, TEXT('a'));
    FBBBMonsterStreamRecord Damage;
    Damage.Kind = EKind::InitialDamage;
    Damage.Action.InstanceId = Motion.Action.InstanceId;
    Damage.Damage.PlayerId = 23;
    Damage.Damage.Parts.Head = 93.5;
    Damage.Damage.LastHitResult.Sequence = 51;

    for (int32 Budget : {FBBBMonsterStreamBatch::MotionBytes, FBBBMonsterStreamBatch::ReliableBytes})
    {
        FBBBMonsterStreamBatch Outgoing;
        Outgoing.Sequence = 42;
        const bool bReliable = Budget == FBBBMonsterStreamBatch::ReliableBytes;
        if (bReliable)
        {
            TestTrue(TEXT("Maximum-length path fits"), Outgoing.TryAdd(Birth, Budget));
            TestTrue(TEXT("Cumulative contribution fits"), Outgoing.TryAdd(Damage, Budget));
        }
        for (int32 Repeat = 0; Repeat < FBBBMonsterStreamBatch::MaxRecords; ++Repeat)
        {
            if (!Outgoing.TryAdd(Motion, Budget))
            {
                break;
            }
        }
        const int32 Before = Outgoing.PayloadBits;
        TestFalse(TEXT("A full batch rejects another motion record"), Outgoing.TryAdd(Motion, Budget));
        TestEqual(TEXT("A rejected record leaves its size intact"), Outgoing.PayloadBits, Before);
        bool bSuccess = false;
        FBitWriter Writer(Budget * 8);
        Outgoing.NetSerialize(Writer, nullptr, bSuccess);
        TestTrue(TEXT("Output fits the actual wire budget"), bSuccess && !Writer.IsError());
        TestEqual(TEXT("Measured and written lengths match"), Writer.GetNumBits(), int64(Before));
        if (!bSuccess || Writer.IsError())
        {
            return false;
        }
        FBitReader Reader(Writer.GetData(), Writer.GetNumBits());
        FBBBMonsterStreamBatch Incoming;
        Incoming.NetSerialize(Reader, nullptr, bSuccess);
        TestTrue(TEXT("The stored batch decodes exactly"), bSuccess && Reader.AtEnd());
        TestEqual(TEXT("Batch sequence survives"), Incoming.Sequence, Outgoing.Sequence);
        TestEqual(TEXT("Record count survives"), Incoming.Records.Num(), Outgoing.Records.Num());
        if (!bSuccess || Incoming.Records.Num() < 3)
        {
            return false;
        }
        const auto& Actual = Incoming.Records.Last().Action;
        TestEqual(TEXT("Instance identity survives"), Actual.InstanceId, Motion.Action.InstanceId);
        TestTrue(TEXT("Position precision is preserved"), Actual.Location.Equals(Motion.Action.Location, 0.051));
        TestTrue(TEXT("Traversal endpoint survives"), Actual.Traversal.End.Equals(Motion.Action.Traversal.End, 0.051));
        TestEqual(TEXT("Rush distance survives"), Actual.Special.RushDistanceCm, Motion.Action.Special.RushDistanceCm);
        if (bReliable)
        {
            TestEqual(TEXT("Definition path is lossless"), Incoming.Records[0].DefinitionPath, Birth.DefinitionPath);
            TestEqual(TEXT("Blocked recovery fits the same phase field"), Incoming.Records[0].Action.Special.Phase, uint8(7));
            TestEqual(TEXT("Part damage is lossless"), Incoming.Records[1].Damage.Parts.Head, 93.5);
            TestTrue(TEXT("Initial damage remains distinct from fresh hits"), Incoming.Records[1].Kind == EKind::InitialDamage);
        }
    }
    return true;
}

#endif
