#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "MassCommonFragments.h"
#include "MassEntityConfigAsset.h"
#include "BBBMonsterDirectorScheduleProcessor.h"
#include "BBBMonsterDirectorParseProcessor.h"
#include "BBBMonsterDirectorSpawnInitializationProcessor.h"
#include "BBBMonsterSpawnRegionEQSGenerator.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterSpawnOwnershipFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterInitializationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

namespace
{
    UWorld* CreateDirectorTestWorld()
    {
        const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false)
            .CreateNavigation(false).CreateAISystem(false);
        UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
        return World;
    }

    void DestroyDirectorTestWorld(UWorld* World)
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    }

    void RunDirectorTestProcessor(UWorld* World, FMassEntityManager& Manager, UClass* Type)
    {
        UMassProcessor* Processor = NewObject<UMassProcessor>(World, Type);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    }
}

/** 验证完整批次与补员差异 申请取消与网络旧版本隔离 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDirectorBudgetTest, "UBBB.Mass.MonsterDirector.BudgetAndMessages",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDirectorBudgetTest::RunTest(const FString& Parameters)
{
    FBBBMonsterDirectorStageDefinition Stage;
    Stage.TotalCount = 12;
    Stage.BatchCount = 5;
    Stage.MaxAlive = 10;
    TestEqual(TEXT("波次等待完整批次容量"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 8, 10), 0);
    TestEqual(TEXT("波次等待完整世界容量"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 0, 4), 0);
    TestEqual(TEXT("最后一波只使用剩余额度"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 10, 0, 2), 2);
    Stage.Mode = EBBBMonsterDirectorSpawnMode::Refill;
    TestEqual(TEXT("补员允许填充少量空位"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 8, 10), 2);
    TestEqual(TEXT("补员不会越过世界上限"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 0, 3), 3);
    TestEqual(TEXT("世界已超上限禁止生成"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 0, -1), 0);
    TestEqual(TEXT("耗尽总额度禁止生成"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 12, 0, 10), 0);
    Stage.Mode = EBBBMonsterDirectorSpawnMode::Once;
    Stage.TotalCount = 10;
    TestEqual(TEXT("一次生成等待完整容量"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 0, 9), 0);
    TestEqual(TEXT("一次生成全部额度"), UBBBMonsterDirectorScheduleProcessor::CalculateRequest(Stage, 0, 0, 10), 10);

    auto* Definition = NewObject<UBBBMonsterDirectorDefinition>();
    Definition->Stages.Add(Stage);
    FBBBMonsterDirectorFragment State;
    State.InstanceId = FGuid::NewGuid();
    State.Definition = Definition;
    State.Generation = 1;
    State.StageIndex = 0;
    State.Phase = EBBBMonsterDirectorPhase::Spawning;
    FBBBMonsterDirectorSpawnFragment Spawn;
    Spawn.RequestId = 7;
    Spawn.Reserved = 5;
    FBBBMonsterDirectorQueryResultLocalControlPacket Result;
    Result.InstanceId = State.InstanceId;
    Result.Generation = 1;
    Result.StageIndex = 0;
    Result.RequestId = 7;
    Result.Locations.Add(FVector(1.0, 2.0, 3.0));
    TestTrue(TEXT("当前申请允许还原查询结果"), Result.IsValid() && Result.CanApply(State, Spawn));
    FBBBMonsterDirectorPauseLocalControlPacket Pause;
    Pause.Apply(State, Spawn);
    TestEqual(TEXT("暂停释放预留"), Spawn.Reserved, 0);
    TestFalse(TEXT("暂停拒绝迟到查询"), Result.CanApply(State, Spawn));
    FBBBMonsterDirectorResumeLocalControlPacket Resume;
    Resume.Apply(State, Spawn);
    Spawn.Reserved = 5;
    TestFalse(TEXT("继续后旧申请仍无效"), Result.CanApply(State, Spawn));
    Result.RequestId = Spawn.RequestId;
    Spawn.bCreating = true;
    FBBBMonsterDirectorCreatedLocalControlPacket Created;
    Created.InstanceId = State.InstanceId;
    Created.Generation = State.Generation;
    Created.StageIndex = 0;
    Created.RequestId = Spawn.RequestId;
    Created.Count = 2;
    Created.Time = 1.0;
    TestTrue(TEXT("允许结算实际成功数量"), Created.CanApply(State, Spawn));
    Created.Apply(State, Spawn);
    TestEqual(TEXT("只扣除实际生成额度"), State.TotalSpawned, 2);
    TestEqual(TEXT("失败位置也释放预留"), Spawn.Reserved, 0);
    TestFalse(TEXT("重复创建结果不能重复扣除"), Created.CanApply(State, Spawn));
    FBBBMonsterDirectorStopLocalControlPacket Stop;
    Stop.Apply(State, Spawn);
    FBBBMonsterDirectorStartLocalControlPacket Start;
    State.EntityCount = 1;
    TestFalse(TEXT("旧实体仍在时禁止重开"), Start.CanApply(State, Spawn));
    State.EntityCount = 0;
    Start.Apply(State, Spawn);
    TestFalse(TEXT("新一代拒绝旧出生结果"), Created.CanApply(State, Spawn));

    FBBBMonsterDirectorStateAuthorityFactPacket Fact;
    Fact.State.InstanceId = State.InstanceId;
    Fact.State.Revision = 4;
    FBBBMonsterDirectorFragment Mirror;
    TestTrue(TEXT("空镜像可以接收当前事实"), Fact.IsValid() && Fact.CanApply(Mirror, Spawn));
    Fact.Apply(Mirror, Spawn);
    TestFalse(TEXT("镜像拒绝重复版本"), Fact.CanApply(Mirror, Spawn));
    Fact.State.Revision = 3;
    TestFalse(TEXT("镜像拒绝旧版本"), Fact.CanApply(Mirror, Spawn));
    Fact.State.Revision = 5;
    Fact.State.InstanceId = FGuid::NewGuid();
    TestFalse(TEXT("镜像拒绝其他实例"), Fact.CanApply(Mirror, Spawn));
    return true;
}

/** 在真实 Mass 查询中验证多导演预留 世界上限 死亡与阶段收尾 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDirectorScheduleTest, "UBBB.Mass.MonsterDirector.SchedulerBudgetAndLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDirectorScheduleTest::RunTest(const FString& Parameters)
{
    UWorld* World = CreateDirectorTestWorld();
    ON_SCOPE_EXIT
    {
        DestroyDirectorTestWorld(World);
    };
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    const auto DirectorType = Manager.CreateArchetype({FBBBMonsterDirectorTag::StaticStruct(), FBBBMonsterDirectorFragment::StaticStruct(),
        FBBBMonsterDirectorSpawnFragment::StaticStruct(), FBBBMonsterDirectorInputFragment::StaticStruct()});
    const auto MonsterType = Manager.CreateArchetype({FBBBMonsterTag::StaticStruct(), FBBBMonsterHealthFragment::StaticStruct(),
        FBBBMonsterSpawnOwnershipFragment::StaticStruct()});
    auto* Definition = NewObject<UBBBMonsterDirectorDefinition>(World);
    Definition->WorldMaxAlive = 8;
    FBBBMonsterDirectorStageDefinition Stage;
    Stage.Mode = EBBBMonsterDirectorSpawnMode::Refill;
    Stage.TotalCount = 20;
    Stage.MaxAlive = 8;
    Definition->Stages.Add(Stage);
    Stage.Id = TEXT("Stage02");
    Stage.Activation = EBBBMonsterDirectorActivation::External;
    Definition->Stages.Add(Stage);
    const FMassEntityHandle A = Manager.CreateEntity(DirectorType);
    const FMassEntityHandle B = Manager.CreateEntity(DirectorType);
    for (const auto Entity : {A, B})
    {
        auto& State = Manager.GetFragmentDataChecked<FBBBMonsterDirectorFragment>(Entity);
        State.InstanceId = FGuid::NewGuid();
        State.Definition = Definition;
        State.StageIndex = 0;
        State.Generation = 1;
        State.Phase = EBBBMonsterDirectorPhase::Spawning;
    }
    const auto AddMonster = [&Manager, MonsterType](const FGuid& DirectorId, const float Health)
    {
        const auto Entity = Manager.CreateEntity(MonsterType);
        Manager.GetFragmentDataChecked<FBBBMonsterSpawnOwnershipFragment>(Entity).DirectorId = DirectorId;
        Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth = Health;
        return Entity;
    };
    const FGuid AId = Manager.GetFragmentDataChecked<FBBBMonsterDirectorFragment>(A).InstanceId;
    const FGuid BId = Manager.GetFragmentDataChecked<FBBBMonsterDirectorFragment>(B).InstanceId;
    const auto LivingA = AddMonster(AId, 100.0f);
    AddMonster(BId, 100.0f);
    const auto CorpseA = AddMonster(AId, 0.0f);
    AddMonster(FGuid(), 100.0f);
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorScheduleProcessor::StaticClass());
    auto& AState = Manager.GetFragmentDataChecked<FBBBMonsterDirectorFragment>(A);
    auto& ASpawn = Manager.GetFragmentDataChecked<FBBBMonsterDirectorSpawnFragment>(A);
    auto& BSpawn = Manager.GetFragmentDataChecked<FBBBMonsterDirectorSpawnFragment>(B);
    TestEqual(TEXT("世界三只存活时最多预留五只"), ASpawn.Reserved + BSpawn.Reserved, 5);
    TestEqual(TEXT("尸体不占用存活额度"), AState.Alive, 1);
    TestEqual(TEXT("尸体仍计入导演所属实体"), AState.EntityCount, 2);

    FBBBMonsterDirectorQueryResultLocalControlPacket Result;
    Result.InstanceId = AId;
    Result.Generation = 1;
    Result.StageIndex = 0;
    Result.RequestId = ASpawn.RequestId;
    Mass->SubmitInput(A, Result);
    Mass->SubmitInput(A, FBBBMonsterDirectorPauseLocalControlPacket());
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorParseProcessor::StaticClass());
    TestTrue(TEXT("同轮暂停优先于查询完成"), AState.Phase == EBBBMonsterDirectorPhase::Paused && !ASpawn.bResultReady && ASpawn.Reserved == 0);
    Mass->SubmitInput(A, FBBBMonsterDirectorStopLocalControlPacket());
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorParseProcessor::StaticClass());
    TestTrue(TEXT("停止优先于后续输入"), AState.Phase == EBBBMonsterDirectorPhase::Stopped && ASpawn.Reserved == 0);

    AState.Phase = EBBBMonsterDirectorPhase::Spawning;
    AState.StageSpawned = 20;
    AState.TotalSpawned = 20;
    AState.NextUpdateTime = 0.0;
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorScheduleProcessor::StaticClass());
    TestTrue(TEXT("总额度用完仍需等待存活清零"), AState.Phase == EBBBMonsterDirectorPhase::Clearing);
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(LivingA).CurrentHealth = 0.0f;
    AState.NextUpdateTime = 0.0;
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorScheduleProcessor::StaticClass());
    TestTrue(TEXT("清场后按顺序进入下一阶段"), AState.Phase == EBBBMonsterDirectorPhase::Waiting && AState.StageIndex == 1);
    FBBBMonsterDirectorTriggerLocalControlPacket Trigger;
    Trigger.Generation = AState.Generation;
    Trigger.StageIndex = 1;
    Mass->SubmitInput(A, Trigger);
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorParseProcessor::StaticClass());
    TestTrue(TEXT("外部输入激活指定阶段"), AState.Phase == EBBBMonsterDirectorPhase::Spawning);
    AState.StageSpawned = 20;
    AState.TotalSpawned = 40;
    AState.NextUpdateTime = 0.0;
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorScheduleProcessor::StaticClass());
    TestTrue(TEXT("尸体存在仍允许当前副本完成"), AState.Phase == EBBBMonsterDirectorPhase::Completed && AState.Alive == 0);
    FBBBMonsterDirectorStartLocalControlPacket Start;
    TestFalse(TEXT("完成后尸体未回收不能重开"), Start.CanApply(AState, ASpawn));
    Manager.DestroyEntity(LivingA);
    Manager.DestroyEntity(CorpseA);
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorScheduleProcessor::StaticClass());
    TestTrue(TEXT("旧实体回收后允许新一轮"), Start.CanApply(AState, ASpawn));

    auto& BState = Manager.GetFragmentDataChecked<FBBBMonsterDirectorFragment>(B);
    BSpawn.Reserved = 2;
    BSpawn.StartedAt = -20.0;
    BSpawn.bCreating = false;
    BState.NextUpdateTime = 0.0;
    const uint32 OldRequest = BSpawn.RequestId;
    RunDirectorTestProcessor(World, Manager, UBBBMonsterDirectorScheduleProcessor::StaticClass());
    TestTrue(TEXT("超时释放额度并废弃申请"), BSpawn.Reserved == 0 && BSpawn.RequestId > OldRequest);
    return true;
}

/** 验证有界采样始终处于变换后的方盒与圆柱内 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDirectorRegionTest, "UBBB.Mass.MonsterDirector.RegionSampling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDirectorRegionTest::RunTest(const FString& Parameters)
{
    FBBBMonsterSpawnRegionDefinition Region;
    Region.Transform = FTransform(FRotator(10.0, 35.0, 15.0), FVector(700.0, -500.0, 200.0), FVector(2.0, 0.5, 1.5));
    for (const auto Shape : {EBBBMonsterSpawnRegionShape::Box, EBBBMonsterSpawnRegionShape::Cylinder})
    {
        Region.Shape = Shape;
        TArray<FVector> Points;
        TArray<FVector> Repeated;
        UBBBMonsterSpawnRegionEQSGenerator::GenerateCandidates({Region}, 1000, 42, Points);
        UBBBMonsterSpawnRegionEQSGenerator::GenerateCandidates({Region}, 1000, 42, Repeated);
        TestEqual(TEXT("采样数量有界"), Points.Num(), 512);
        TestTrue(TEXT("同种子可重现预览"), Points == Repeated);
        for (const auto& Point : Points)
        {
            TestTrue(TEXT("变换后候选点位于区域内"), Region.Contains(Point));
        }
    }
    Region.Extent.X = 0.0;
    TestFalse(TEXT("零尺寸区域拒绝"), Region.IsValid());
    Region.Extent.X = 1000.0;
    Region.Shape = static_cast<EBBBMonsterSpawnRegionShape>(255);
    TestFalse(TEXT("无效形状拒绝"), Region.IsValid());
    return true;
}

/** 复用实际小怪配置验证批量出生只初始化本批 归属与个体初始化继续成立 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDirectorBatchTest, "UBBB.Mass.MonsterDirector.BatchSpawn",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDirectorBatchTest::RunTest(const FString& Parameters)
{
    const auto* Definition = LoadObject<UBBBMonsterDefinition>(nullptr,
        TEXT("/Game/_Project/Mass/Monster/Zombie/Male/Variants/Connor/DA_BBBZombieConnorDefinition"));
    if (!TestNotNull(TEXT("现有康纳小怪定义"), Definition) || !TestTrue(TEXT("现有小怪配置有效"), Definition->IsValid()))
    {
        return false;
    }
    UWorld* World = CreateDirectorTestWorld();
    ON_SCOPE_EXIT
    {
        DestroyDirectorTestWorld(World);
    };
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    const auto Old = Mass->CreateEntity(*Definition->EntityConfig);
    const FTransform OldTransform(FVector(-1000.0, 0.0, 90.0));
    Manager.GetFragmentDataChecked<FTransformFragment>(Old).GetMutableTransform() = OldTransform;
    FBBBMonsterDirectorSpawnData Data;
    Data.DirectorId = FGuid::NewGuid();
    Data.Generation = 3;
    Data.StageIndex = 1;
    Data.Transforms.Add(FTransform(FVector(100.0, 200.0, 90.0)));
    Data.Transforms.Add(FTransform(FVector(300.0, 400.0, 90.0)));
    TArray<FMassEntityHandle> Created;
    TestTrue(TEXT("原生批量出生成功"), Mass->CreateEntities(*Definition->EntityConfig, 2, FConstStructView::Make(Data),
        UBBBMonsterDirectorSpawnInitializationProcessor::StaticClass(), Created));
    TestEqual(TEXT("实际出生两只"), Created.Num(), 2);
    TestTrue(TEXT("本批初始化不触碰既有实体"), Manager.GetFragmentDataChecked<FTransformFragment>(Old).GetTransform().Equals(OldTransform));
    for (int32 Index = 0; Index < Created.Num(); ++Index)
    {
        const auto& Owner = Manager.GetFragmentDataChecked<FBBBMonsterSpawnOwnershipFragment>(Created[Index]);
        TestTrue(TEXT("每只实体保留导演运行和阶段归属"), Owner.DirectorId == Data.DirectorId && Owner.Generation == 3 && Owner.StageIndex == 1);
        TestTrue(TEXT("每只实体使用对应出生位置"), Manager.GetFragmentDataChecked<FTransformFragment>(Created[Index]).GetTransform().Equals(Data.Transforms[Index]));
    }
    RunDirectorTestProcessor(World, Manager, UBBBMonsterInitializationProcessor::StaticClass());
    for (const auto Entity : Created)
    {
        TestTrue(TEXT("现有初始化阶段建立网络出生身份"), Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).InstanceId.IsValid());
        TestTrue(TEXT("继续使用现有个体随机初始化"), Manager.GetFragmentDataChecked<FBBBMonsterVariationFragment>(Entity).Seed != 0);
    }
    return true;
}

#endif
