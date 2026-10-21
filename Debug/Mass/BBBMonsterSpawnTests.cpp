#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnPoolDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterSpawnProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterInitializationProcessor.h"

/** 固定种子 独立通道 零权重与感染度配置的回归验收 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSpawnRandomTest, "UBBB.Mass.MonsterSpawn.RandomChannels",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSpawnRandomTest::RunTest(const FString& Parameters)
{
    const auto* Saved = LoadObject<UBBBMonsterSpawnDefinition>(nullptr,
        TEXT("/Game/_Project/Mass/Monster/Spawn/DA_BBBMonsterSpawn_Default"));
    if (!TestNotNull(TEXT("已保存的统一出生配置"), Saved))
    {
        return false;
    }
    auto* Profile = NewObject<UBBBMonsterSpawnDefinition>();
    Profile->Pool = NewObject<UBBBMonsterSpawnPoolDefinition>(Profile);
    Profile->Pool->Entries = Saved->Pool->Entries;
    Profile->Pool->Entries.Last().Weight = 0.0f;
    FBBBMonsterSpawnRegionDefinition Region;
    Region.Transform = FTransform(FRotator(0.0f, 37.0f, 0.0f), FVector(100.0, 300.0, 700.0));
    for (const auto Shape : {EBBBMonsterSpawnRegionShape::Box, EBBBMonsterSpawnRegionShape::Cylinder, EBBBMonsterSpawnRegionShape::Disk})
    {
        Region.Shape = Shape;
        for (int32 Index = 0; Index < 512; ++Index)
        {
            const auto Point = UBBBMonsterSpawnProcessor::Candidate({Region}, 42, Index);
            TestTrue(TEXT("候选始终处于区域"), Region.Contains(Point));
            TestTrue(TEXT("同种子选点复现"), Point == UBBBMonsterSpawnProcessor::Candidate({Region}, 42, Index));
        }
    }
    TSet<UBBBMonsterDefinition*> Seen;
    for (int32 Index = 0; Index < 10000; ++Index)
    {
        auto* Definition = UBBBMonsterSpawnProcessor::SelectDefinition(*Profile, 42, Index);
        Seen.Add(Definition);
        TestTrue(TEXT("零权重模板不生成"), Definition != Profile->Pool->Entries.Last().Definition);
        TestTrue(TEXT("同种子模板复现"), Definition == UBBBMonsterSpawnProcessor::SelectDefinition(*Profile, 42, Index));
        TestTrue(TEXT("独立通道不共享随机流"), UBBBMonsterSpawnProcessor::RandomBits(42, Index, 0x205u)
            != UBBBMonsterSpawnProcessor::RandomBits(42, Index, 0x407u));
    }
    TestEqual(TEXT("其余九种模板都覆盖"), Seen.Num(), 9);
    auto* Rules = NewObject<UBBBMonsterVariationDefinition>();
    auto* Definition = NewObject<UBBBMonsterDefinition>();
    Definition->Variation = Rules;
    Rules->MinInfection = 77;
    Rules->MaxInfection = 77;
    Rules->LocomotionWeights = FVector(0.0, 0.0, 1.0);
    Rules->bRandomPhase = false;
    FBBBMonsterVariationFragment A;
    FBBBMonsterVariationFragment B;
    A.Seed = 123;
    B.Seed = 123;
    FBBBMonsterMovementFragment MovementA;
    FBBBMonsterMovementFragment MovementB;
    FBBBMonsterBehaviorFragment BehaviorA;
    FBBBMonsterBehaviorFragment BehaviorB;
    TestTrue(TEXT("固定出生配置有效"), Rules->IsValid());
    TestTrue(TEXT("房主出生初始化"), UBBBMonsterInitializationProcessor::InitializeBirth(FGuid::NewGuid(), *Definition, A, MovementA, BehaviorA));
    TestTrue(TEXT("客机出生初始化"), UBBBMonsterInitializationProcessor::InitializeBirth(FGuid::NewGuid(), *Definition, B, MovementB, BehaviorB));
    TestTrue(TEXT("随机与网络身份解耦"), A.Infection == B.Infection && A.Seed == B.Seed && MovementA.RunSpeed == MovementB.RunSpeed);
    TestTrue(TEXT("固定感染度 风格与初相成立"), A.Infection == 77 && A.LocomotionStyle == 2 && A.PhaseOffset == 0.0f);
    Rules->LocomotionWeights = FVector::ZeroVector;
    TestFalse(TEXT("全零风格权重拒绝"), Rules->IsValid());
    return true;
}

/** 无导航时验证有界失败 多入口隔离与暂停停止 不依赖渲染 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSpawnBudgetTest, "UBBB.Mass.MonsterSpawn.BudgetAndCancellation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSpawnBudgetTest::RunTest(const FString& Parameters)
{
    const auto* Saved = LoadObject<UBBBMonsterSpawnDefinition>(nullptr,
        TEXT("/Game/_Project/Mass/Monster/Spawn/DA_BBBMonsterSpawn_Default"));
    if (!TestNotNull(TEXT("已保存的出生配置"), Saved))
    {
        return false;
    }
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    auto* Profile = NewObject<UBBBMonsterSpawnDefinition>(World);
    Profile->Pool = Saved->Pool;
    Profile->CandidateCount = 17;
    Profile->CandidatesPerFrame = 3;
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FBBBMonsterSpawnFragment::StaticStruct(), FBBBMonsterSpawnInputFragment::StaticStruct()});
    const auto A = Manager.CreateEntity(Type);
    const auto B = Manager.CreateEntity(Type);
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    FBBBMonsterSpawnRequestLocalControlPacket Packet;
    Packet.OwnerId = FGuid::NewGuid();
    Packet.RequestId = 1;
    Packet.Definition = Profile;
    Packet.Seed = 42;
    Packet.Count = 5;
    Packet.Regions.AddDefaulted();
    TestTrue(TEXT("合法请求有效"), Packet.IsValid());
    Mass->SubmitInput(A, Packet);
    Packet.OwnerId = FGuid::NewGuid();
    Mass->SubmitInput(B, Packet);
    auto* Processor = NewObject<UBBBMonsterSpawnProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    const auto Run = [&Manager, Processor]()
    {
        UE::Mass::FProcessingContext Context(Manager, 1.0f / 30.0f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    Run();
    auto& StateA = Manager.GetFragmentDataChecked<FBBBMonsterSpawnFragment>(A);
    auto& StateB = Manager.GetFragmentDataChecked<FBBBMonsterSpawnFragment>(B);
    TestEqual(TEXT("多入口共用世界候选预算"), StateA.Cursor + StateB.Cursor, 3);
    TestTrue(TEXT("多入口身份互不覆盖"), StateA.OwnerId != StateB.OwnerId);
    Mass->SubmitInput(A, FBBBMonsterSpawnPauseLocalControlPacket());
    const int32 BeforePause = StateA.Cursor;
    Run();
    TestEqual(TEXT("暂停不继续选点"), StateA.Cursor, BeforePause);
    TestTrue(TEXT("其他入口仍推进"), StateB.Cursor > 0);
    Mass->SubmitInput(A, FBBBMonsterSpawnResumeLocalControlPacket());
    Run();
    TestTrue(TEXT("继续后恢复选点"), StateA.Cursor > BeforePause);
    Mass->SubmitInput(A, FBBBMonsterSpawnStopLocalControlPacket());
    Run();
    const int32 StoppedAt = StateA.Cursor;
    Run();
    TestTrue(TEXT("停止后不继续创建"), !StateA.bActive && StateA.Created == 0 && StateA.Cursor == StoppedAt);
    TestFalse(TEXT("旧请求不可重放"), Packet.CanApply(StateA));
    for (int32 Index = 0; Index < 20; ++Index)
    {
        Run();
    }
    TestTrue(TEXT("无合格点有界结束"), !StateB.bActive && StateB.Cursor == 17 && StateB.Created == 0);
    TestTrue(TEXT("明确报告拒绝原因"), StateB.Rejections.Contains(TEXT("没有可用导航位置")));
    return true;
}
#endif
