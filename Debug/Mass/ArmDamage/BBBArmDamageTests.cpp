#include "BBBArmDamageProcessor.h"
#include "Misc/AutomationTest.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    TArray<FBBBArmDamageCell> MakeArm()
    {
        TArray<FBBBArmDamageCell> Cells;
        for (int32 Index = 0; Index <= 200; ++Index)
        {
            FBBBArmDamageCell Cell;
            Cell.Position = FVector(Index * 0.5, 0, 0);
            Cell.Station = Index;
            if (Index > 0)
            {
                Cell.Neighbors.Add(Index - 1);
            }
            if (Index < 200)
            {
                Cell.Neighbors.Add(Index + 1);
            }
            Cells.Add(MoveTemp(Cell));
        }
        return Cells;
    }

    FBBBArmDamageHit HitAt(const double Distance, const float Strength)
    {
        FBBBArmDamageHit Hit;
        Hit.Position = FVector(Distance, -1, 0);
        Hit.Direction = FVector::RightVector;
        Hit.Radius = 1.1;
        Hit.Depth = 2;
        Hit.Strength = Strength;
        return Hit;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBArmDamageAccumulationTest, "UBBB.Debug.ArmDamage.Accumulation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBArmDamageAccumulationTest::RunTest(const FString&)
{
    FBBBArmDamageRuntimeData Runtime;
    TestTrue(TEXT("结构初始化"), FBBBArmDamageProcessor::Initialize(Runtime, MakeArm()));
    const auto& State = Runtime.Damage.ReadArmDamageState();
    TestTrue(TEXT("初始手部连接"), State.bHandPresent && !State.bHandDetached);
    const auto Hit = HitAt(40, 0.6f);
    TestTrue(TEXT("接受第一次命中"), FBBBArmDamageProcessor::ApplyHit(Runtime, Hit));
    TestEqual(TEXT("首次局部损伤保留连接"), State.RemovedCount, 0);
    TestTrue(TEXT("只检查局部采样点"), State.LastVisitedCells < 30);
    FBBBArmDamageProcessor::ApplyHit(Runtime, Hit);
    const int32 AfterTwo = State.RemovedCount;
    TestTrue(TEXT("重复命中打断中部并带走手部"), State.bHandDetached && AfterTwo > 0);
    for (int32 Count = 0; Count < 6; ++Count)
    {
        FBBBArmDamageProcessor::ApplyHit(Runtime, Hit);
    }
    TestTrue(TEXT("同一点连续命中扩大损伤"), State.RemovedCount > AfterTwo);
    TestFalse(TEXT("肩部固定端保持连接"), State.HandConnected[0]);
    TestTrue(TEXT("手部远端仍然存在"), State.HandConnected[200]);
    const auto Damage = State.Damage;
    FBBBArmDamageProcessor::ApplyHit(Runtime, HitAt(70, 2));
    for (int32 Index = 0; Index < Damage.Num(); ++Index)
    {
        TestTrue(TEXT("不同位置的新命中保留旧损伤"), State.Damage[Index] >= Damage[Index]);
    }
    FBBBArmDamageProcessor::Reset(Runtime);
    TestEqual(TEXT("明确重置恢复组织"), State.RemovedCount, 0);
    TestFalse(TEXT("重置恢复手部连接"), State.bHandDetached);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBArmDamageBoundariesTest, "UBBB.Debug.ArmDamage.Boundaries",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBArmDamageBoundariesTest::RunTest(const FString&)
{
    FBBBArmDamageRuntimeData First;
    FBBBArmDamageRuntimeData Second;
    TestTrue(TEXT("第一测试体初始化"), FBBBArmDamageProcessor::Initialize(First, MakeArm()));
    TestTrue(TEXT("第二测试体初始化"), FBBBArmDamageProcessor::Initialize(Second, MakeArm()));
    FBBBArmDamageProcessor::ApplyHit(First, HitAt(25, 2));
    FBBBArmDamageProcessor::ApplyHit(Second, HitAt(75, 2));
    const auto& A = First.Damage.ReadArmDamageState();
    const auto& B = Second.Damage.ReadArmDamageState();
    TestTrue(TEXT("两个不同断点分别由命中决定"), A.Removed[50] && B.Removed[150]);
    TestFalse(TEXT("第二测试体保持前部原样"), B.Removed[50]);
    TestFalse(TEXT("第一测试体保持后部原样"), A.Removed[150]);
    auto Invalid = HitAt(40, 1);
    Invalid.Direction = FVector::ZeroVector;
    TestFalse(TEXT("拒绝无方向命中"), FBBBArmDamageProcessor::ApplyHit(First, Invalid));
    Invalid = HitAt(40, 1);
    Invalid.Radius = -1;
    TestFalse(TEXT("拒绝负毁伤半径"), FBBBArmDamageProcessor::ApplyHit(First, Invalid));
    auto Malformed = MakeArm();
    Malformed[5].Neighbors.Add(999);
    TestFalse(TEXT("拒绝越界制作连接"), FBBBArmDamageProcessor::Initialize(First, MoveTemp(Malformed)));
    TestFalse(TEXT("无效重配清理旧结构"), First.Damage.ReadArmDamageState().Definition.IsValid());
    TestFalse(TEXT("无结构时拒绝命中"), FBBBArmDamageProcessor::ApplyHit(First, HitAt(40, 1)));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBArmDamageAuthoredArmTest, "UBBB.Debug.ArmDamage.AuthoredArm",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBArmDamageAuthoredArmTest::RunTest(const FString&)
{
    FString Json;
    const FString Path = FPaths::ProjectDir() / TEXT("ArtSource/ZombieArmPrototype/LeftArm_Connectivity.json");
    if (!FFileHelper::LoadFileToString(Json, *Path))
    {
        AddError(TEXT("缺少本轮左臂制作连接结构"));
        return false;
    }
    TSharedPtr<FJsonObject> Document;
    const auto Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Document) || !Document
        || Document->GetStringField(TEXT("unit")) != TEXT("centimeter"))
    {
        AddError(TEXT("左臂制作结构格式或单位错误"));
        return false;
    }
    TArray<FBBBArmDamageCell> Cells;
    for (const auto& Value : Document->GetArrayField(TEXT("nodes")))
    {
        const auto Node = Value->AsObject();
        const auto& Position = Node->GetArrayField(TEXT("position"));
        FBBBArmDamageCell Cell;
        Cell.Position = FVector(Position[0]->AsNumber(), Position[1]->AsNumber(), Position[2]->AsNumber());
        Cell.Station = Node->GetIntegerField(TEXT("station"));
        Cells.Add(MoveTemp(Cell));
    }
    for (const auto& Value : Document->GetArrayField(TEXT("edges")))
    {
        const auto& Edge = Value->AsArray();
        const int32 First = static_cast<int32>(Edge[0]->AsNumber());
        const int32 Second = static_cast<int32>(Edge[1]->AsNumber());
        if (!Cells.IsValidIndex(First) || !Cells.IsValidIndex(Second))
        {
            AddError(TEXT("制作连接越界"));
            return false;
        }
        Cells[First].Neighbors.Add(Second);
        Cells[Second].Neighbors.Add(First);
    }
    FVector Elbow = FVector::ZeroVector;
    FVector Wrist = FVector::ZeroVector;
    int32 ElbowCount = 0;
    int32 WristCount = 0;
    for (const auto& Cell : Cells)
    {
        if (Cell.Station == 42)
        {
            Elbow += Cell.Position;
            ++ElbowCount;
        }
        if (Cell.Station == 90)
        {
            Wrist += Cell.Position;
            ++WristCount;
        }
    }
    if (ElbowCount == 0 || WristCount == 0)
    {
        AddError(TEXT("制作结构缺少肘腕截面"));
        return false;
    }
    Elbow /= ElbowCount;
    Wrist /= WristCount;
    const FVector Axis = (Wrist - Elbow).GetSafeNormal();
    const FVector Side = FVector::CrossProduct(Axis, FVector::UpVector).GetSafeNormal();
    const FVector Other = FVector::CrossProduct(Axis, Side).GetSafeNormal();
    FBBBArmDamageRuntimeData Runtime;
    if (!TestTrue(TEXT("实际制作结构完整连通"), FBBBArmDamageProcessor::Initialize(Runtime, Cells)))
    {
        return false;
    }
    const auto& State = Runtime.Damage.ReadArmDamageState();
    TestEqual(TEXT("实际制作采样点数量"), State.Damage.Num(), 9526);
    for (const double Fraction : {0.36, 0.68})
    {
        FBBBArmDamageProcessor::Reset(Runtime);
        int32 LastRemoved = 0;
        int32 SeveredAt = 0;
        for (int32 Shot = 0; Shot < 36; ++Shot)
        {
            const double Angle = Shot * UE_DOUBLE_TWO_PI / 6;
            const FVector Center = FMath::Lerp(Elbow, Wrist, Fraction)
                + (Side * FMath::Cos(Angle) + Other * FMath::Sin(Angle)) * 1.8;
            auto Hit = HitAt(0, 2);
            Hit.Direction = Side;
            Hit.Position = Center - Side * 2.3;
            Hit.Radius = 2.3;
            Hit.Depth = 4.6;
            FBBBArmDamageProcessor::ApplyHit(Runtime, Hit);
            TestTrue(TEXT("制作左臂的损毁持续累加"), State.RemovedCount >= LastRemoved);
            TestTrue(TEXT("实际局部命中减少候选访问"), State.LastVisitedCells < Cells.Num() / 2);
            LastRemoved = State.RemovedCount;
            if (State.bHandDetached && SeveredAt == 0)
            {
                SeveredAt = Shot + 1;
            }
        }
        TestTrue(TEXT("实际手部在累计破坏后失去肩部连接"), State.bHandPresent && State.bHandDetached);
        AddInfo(FString::Printf(TEXT("[BBBArmDamage] 断点比例 %.2f 第 %d 次命中失去连接 累计移除 %d"),
            Fraction, SeveredAt, State.RemovedCount));
    }
    TArray<double> Samples;
    int32 MaximumVisited = 0;
    for (int32 Shot = 0; Shot < 1000; ++Shot)
    {
        if (Shot % 32 == 0)
        {
            FBBBArmDamageProcessor::Reset(Runtime);
        }
        const double Fraction = 0.15 + 0.7 * (Shot % 29) / 28.0;
        auto Hit = HitAt(0, 2);
        Hit.Direction = Side;
        Hit.Position = FMath::Lerp(Elbow, Wrist, Fraction) - Side * 2.3;
        Hit.Radius = 2.3;
        Hit.Depth = 4.6;
        const double Start = FPlatformTime::Seconds();
        FBBBArmDamageProcessor::ApplyHit(Runtime, Hit);
        Samples.Add((FPlatformTime::Seconds() - Start) * 1000);
        MaximumVisited = FMath::Max(MaximumVisited, State.LastVisitedCells);
    }
    Samples.Sort();
    double Mean = 0;
    for (const double Sample : Samples)
    {
        Mean += Sample / Samples.Num();
    }
    AddInfo(FString::Printf(TEXT("[BBBArmDamagePerf] hits=%d mean_ms=%.6f p95_ms=%.6f max_ms=%.6f max_candidates=%d total_cells=%d scope=CPU_CORE_ONLY"),
        Samples.Num(), Mean, Samples[949], Samples.Last(), MaximumVisited, Cells.Num()));
    return true;
}

#endif
