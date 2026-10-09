#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "HAL/PlatformTime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "MassEntityConfigAsset.h"
#include "MassSpawnerTypes.h"
#include "MassCommonUtils.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterSpawnGenerator.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterInitializationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterInitializationPendingTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationStateProcessor.h"

/** 验证固定出生随机 跨端还原 分布与仅出生执行的查询 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterVariationTest, "UBBB.Mass.ZombieVariation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterVariationTest::RunTest(const FString& Parameters)
{
    auto* Rules = NewObject<UBBBMonsterVariationDefinition>();
    auto* Definition = NewObject<UBBBMonsterDefinition>();
    Definition->Variation = Rules;
    TestTrue(TEXT("默认共享出生规则有效"), Rules->IsValid());
    FBBBMonsterVariationFragment UnreadyVariation;
    FBBBMonsterMovementFragment UnreadyMovement;
    FBBBMonsterBehaviorFragment UnreadyBehavior;
    TestFalse(TEXT("未收到稳定身份时禁止初始化"), UBBBMonsterInitializationProcessor::InitializeBirth(FGuid(), *Definition, UnreadyVariation, UnreadyMovement, UnreadyBehavior));
    TestEqual(TEXT("等待网络身份不能自行生成个体种子"), UnreadyVariation.Seed, 0u);
    TestTrue(TEXT("固定个体数据不超过三十二字节"), sizeof(FBBBMonsterVariationFragment) <= 32);
    int32 Runners = 0;
    int32 LowCount = 0;
    int32 LowRunners = 0;
    int32 HighCount = 0;
    int32 HighRunners = 0;
    TSet<uint32> Seeds;
    TSet<uint8> Styles;
    for (int32 Index = 0; Index < 10000; ++Index)
    {
        const FGuid Identity(0x31415926u, static_cast<uint32>(Index + 1), 0x27182818u, 1u);
        FBBBMonsterVariationFragment Host;
        FBBBMonsterVariationFragment Client;
        FBBBMonsterMovementFragment Movement;
        FBBBMonsterMovementFragment RemoteMovement;
        FBBBMonsterBehaviorFragment Behavior;
        FBBBMonsterBehaviorFragment RemoteBehavior;
        if (!TestTrue(TEXT("有效出生身份完成初始化"), UBBBMonsterInitializationProcessor::InitializeBirth(Identity, *Definition, Host, Movement, Behavior)))
        {
            return false;
        }
        UBBBMonsterInitializationProcessor::InitializeBirth(Identity, *Definition, Client, RemoteMovement, RemoteBehavior);
        TestTrue(TEXT("跨端个体属性确定性还原"), Host.Seed == Client.Seed && Host.Infection == Client.Infection && Host.LocomotionStyle == Client.LocomotionStyle
            && Host.PhaseOffset == Client.PhaseOffset && Movement.WalkSpeed == RemoteMovement.WalkSpeed && Movement.MaxGait == RemoteMovement.MaxGait
            && Behavior.AlertDuration == RemoteBehavior.AlertDuration);
        TestTrue(TEXT("随机速度有界且档位顺序不变"), Host.SpeedScale >= 0.85f && Host.SpeedScale <= 1.15f
            && Movement.WalkSpeed < Movement.RunSpeed && Movement.RunSpeed < Movement.SprintSpeed);
        TestTrue(TEXT("循环初相有界"), Host.PhaseOffset >= 0.0f && Host.PhaseOffset < 1.0f);
        const bool bRunner = Movement.MaxGait != EBBBMonsterGait::Walk;
        Runners += bRunner ? 1 : 0;
        if (Host.Infection < 64)
        {
            ++LowCount;
            LowRunners += bRunner ? 1 : 0;
        }
        if (Host.Infection >= 192)
        {
            ++HighCount;
            HighRunners += bRunner ? 1 : 0;
        }
        if (!bRunner)
        {
            TestTrue(TEXT("走尸远近追击都不能升级跑步"), UBBBMonsterLocomotionProcessor::SelectGait(Movement, 5000.0f, false) == EBBBMonsterGait::Walk
                && UBBBMonsterLocomotionProcessor::SelectGait(Movement, 1.0f, false) == EBBBMonsterGait::Walk);
        }
        Seeds.Add(Host.Seed);
        Styles.Add(Host.LocomotionStyle);
    }
    TestTrue(TEXT("大样本跑尸比例接近两成"), Runners >= 1700 && Runners <= 2300);
    TestTrue(TEXT("低感染跑尸概率明显高于高感染"), LowCount > 2000 && HighCount > 2000
        && static_cast<float>(LowRunners) / LowCount > static_cast<float>(HighRunners) / HighCount + 0.2f);
    TestTrue(TEXT("出生种子不形成重复批次"), Seeds.Num() > 9900);
    TestEqual(TEXT("移动风格覆盖三种"), Styles.Num(), 3);
    Rules->HighInfectionRunnerChance = 0.8f;
    TestFalse(TEXT("反向感染概率配置拒绝"), Rules->IsValid());
    Rules->HighInfectionRunnerChance = 0.02f;

    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("出生查询隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    AActor* SpawnOwner = World->SpawnActor<AActor>();
    auto* Generator = NewObject<UBBBMonsterSpawnGenerator>(SpawnOwner);
    TArray<FMassSpawnedEntityType> AppearanceTypes;
    for (int32 Index = 0; Index < 10; ++Index)
    {
        auto& Appearance = AppearanceTypes.AddDefaulted_GetRef();
        Appearance.EntityConfig = NewObject<UMassEntityConfigAsset>(Generator);
        Appearance.Proportion = Index == 9 ? 0.0f : 1.0f;
    }
    TArray<int32> AppearanceCounts;
    AppearanceCounts.SetNumZeroed(10);
    FFinishedGeneratingSpawnDataSignature Finished = FFinishedGeneratingSpawnDataSignature::CreateLambda(
        [&AppearanceCounts](TConstArrayView<FMassEntitySpawnDataGeneratorResult> Results)
        {
            for (const auto& Result : Results)
            {
                AppearanceCounts[Result.EntityConfigIndex] += Result.NumEntities;
            }
        });
    Generator->Generate(*SpawnOwner, AppearanceTypes, 10000, Finished);
    int32 AppearanceTotal = 0;
    for (int32 Index = 0; Index < 9; ++Index)
    {
        AppearanceTotal += AppearanceCounts[Index];
        TestTrue(TEXT("有效外观模板随机覆盖且批次计数有界"), AppearanceCounts[Index] >= 850 && AppearanceCounts[Index] <= 1350);
    }
    TestEqual(TEXT("权重抽样保留请求出生总数"), AppearanceTotal, 10000);
    TestEqual(TEXT("零权重外观不生成"), AppearanceCounts[9], 0);
    SpawnOwner->Destroy();
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto LoopType = Manager.CreateArchetype({FBBBMonsterTag::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterVariationFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct(),
        FBBBMonsterCombatFragment::StaticStruct(), FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterPresentationStateFragment::StaticStruct()});
    const auto LoopEntity = Manager.CreateEntity(LoopType);
    Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(LoopEntity).Definition = Definition;
    auto& LoopVariation = Manager.GetFragmentDataChecked<FBBBMonsterVariationFragment>(LoopEntity);
    LoopVariation.Seed = 17;
    LoopVariation.PhaseOffset = 0.25f;
    LoopVariation.SpeedScale = 1.1f;
    LoopVariation.LocomotionStyle = 2;
    auto& LoopBehavior = Manager.GetFragmentDataChecked<FBBBMonsterBehaviorFragment>(LoopEntity);
    LoopBehavior.State = EBBBMonsterBehavior::Chase;
    Manager.GetFragmentDataChecked<FMassVelocityFragment>(LoopEntity).Value = FVector(Definition->WalkSpeed * 1.1f, 0.0f, 0.0f);
    Rules->MovementCycleSeconds[2].X = 4.0;
    auto* SnapshotProcessor = NewObject<UBBBMonsterPresentationStateProcessor>(World);
    SnapshotProcessor->CallInitialize(World, Manager.AsShared());
    const auto SampleLoop = [&Manager, SnapshotProcessor]()
    {
        UE::Mass::FProcessingContext Context(Manager, 0.1f);
        UE::Mass::Executor::Run(*SnapshotProcessor, Context);
    };
    SampleLoop();
    TestTrue(TEXT("长循环按真实片段时长推进而非统一一秒"), FMath::IsNearlyEqual(
        Manager.GetFragmentDataChecked<FBBBMonsterPresentationStateFragment>(LoopEntity).LoopPhase, 0.2775f));
    LoopBehavior.State = EBBBMonsterBehavior::Idle;
    SampleLoop();
    TestTrue(TEXT("状态切换不重置循环起点 且无演员时继续推进"), FMath::IsNearlyEqual(
        Manager.GetFragmentDataChecked<FBBBMonsterPresentationStateFragment>(LoopEntity).LoopPhase, 0.3775f));
    Manager.DestroyEntity(LoopEntity);
    const auto Type = Manager.CreateArchetype({FBBBMonsterTag::StaticStruct(), FBBBMonsterInitializationPendingTag::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterVariationFragment::StaticStruct(),
        FBBBMonsterMovementFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct()});
    auto* Processor = NewObject<UBBBMonsterInitializationProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    for (const int32 Count : {50, 200, 500})
    {
        TArray<FMassEntityHandle> Entities;
        for (int32 Index = 0; Index < Count; ++Index)
        {
            const auto Entity = Manager.CreateEntity(Type);
            Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = Definition;
            Entities.Add(Entity);
        }
        const auto Run = [&Manager, Processor]()
        {
            UE::Mass::FProcessingContext Context(Manager, 1.0f / 30.0f);
            UE::Mass::Executor::Run(*Processor, Context);
        };
        const double Start = FPlatformTime::Seconds();
        Run();
        const double BirthMs = (FPlatformTime::Seconds() - Start) * 1000.0;
        for (const auto Entity : Entities)
        {
            TestTrue(TEXT("出生身份前移且待初始化标签移除"), Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).InstanceId.IsValid()
                && !Manager.GetArchetypeComposition(Manager.GetArchetypeForEntity(Entity)).Contains<FBBBMonsterInitializationPendingTag>());
        }
        const auto First = Manager.GetFragmentDataChecked<FBBBMonsterVariationFragment>(Entities[0]);
        const double IdleStart = FPlatformTime::Seconds();
        for (int32 Frame = 0; Frame < 300; ++Frame)
        {
            Run();
        }
        const double MeanMs = (FPlatformTime::Seconds() - IdleStart) * 1000.0 / 300.0;
        TestEqual(TEXT("存活期间不重新抽样"), Manager.GetFragmentDataChecked<FBBBMonsterVariationFragment>(Entities[0]).Seed, First.Seed);
        AddInfo(FString::Printf(TEXT("[BBBVariation]Count=%d BirthMs=%.4f EmptyQueryMeanMs=%.6f FragmentBytes=%llu Runners=%d/10000"),
            Count, BirthMs, MeanMs, static_cast<uint64>(sizeof(FBBBMonsterVariationFragment)), Runners));
        Manager.BatchDestroyEntities(Entities);
    }
    return true;
}

#endif
