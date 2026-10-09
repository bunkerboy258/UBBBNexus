#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "HAL/PlatformTime.h"
#include "HAL/IConsoleManager.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "MassSpawner.h"
#include "MassEntityConfigAsset.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "MassEntityQuery.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterStimulusFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassValidationLibrary.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Perception/BBBMonsterPerceptionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Input/BBBMonsterParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Perception/FBBBMonsterSoundLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

/** 验证严格视觉 渐进确认 稳定选敌 当前线索与五十只批量感知 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterPerceptionTest, "UBBB.Mass.ZombiePerception",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterPerceptionTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("感知隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FBBBMonsterPerceptionFragment::StaticStruct(),
        FBBBMonsterTargetFragment::StaticStruct(), FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterNavigationFragment::StaticStruct(), FBBBMonsterStimulusFragment::StaticStruct(),
        FBBBMonsterPerceptionInputFragment::StaticStruct(), FBBBMonsterTag::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterHealthInputFragment::StaticStruct(), FBBBMonsterNetworkInputFragment::StaticStruct(),
        FBBBMonsterHitReactionInputFragment::StaticStruct(), FBBBMonsterHitReactionFragment::StaticStruct(),
        FBBBMonsterBehaviorFragment::StaticStruct()});
    auto* Settings = NewObject<UBBBMonsterDefinition>(World);
    Settings->AllyAlertRange = 0.0f;
    auto* Processor = NewObject<UBBBMonsterPerceptionProcessor>(World);
    Processor->CallInitialize(World, Manager.AsShared());
    auto* Parser = NewObject<UBBBMonsterParseProcessor>(World);
    Parser->CallInitialize(World, Manager.AsShared());
    const auto Create = [&Manager, Type, Settings](const FVector& Position, const float Yaw = 0.0f)
    {
        const auto Entity = Manager.CreateEntity(Type);
        Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform() = FTransform(FRotator(0.0f, Yaw, 0.0f), Position);
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = Settings;
        return Entity;
    };
    const auto MakePlayer = [World](const FVector& Position)
    {
        APawn* Pawn = World->SpawnActor<APawn>();
        auto* Capsule = NewObject<UCapsuleComponent>(Pawn);
        Pawn->SetRootComponent(Capsule);
        Capsule->SetCapsuleSize(34.0f, 90.0f);
        Capsule->SetCollisionProfileName(TEXT("Pawn"));
        Capsule->RegisterComponent();
        Pawn->SetActorLocation(Position);
        World->SpawnActor<APlayerController>()->Possess(Pawn);
        Pawn->SetCanBeDamaged(true);
        return Pawn;
    };
    const auto Entity = Create(FVector(0.0f, 0.0f, 90.0f));
    auto& Sense = Manager.GetFragmentDataChecked<FBBBMonsterPerceptionFragment>(Entity);
    auto& Target = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Entity);
    const auto Run = [&Manager, Processor, World](const int32 Steps = 1)
    {
        for (int32 Step = 0; Step < Steps; ++Step)
        {
            World->TimeSeconds += 0.11;
            UE::Mass::FProcessingContext Context(Manager, 0.11f);
            UE::Mass::Executor::Run(*Processor, Context);
        }
    };
    const auto Reset = [&Sense, &Target]()
    {
        Sense = FBBBMonsterPerceptionFragment{};
        Target = FBBBMonsterTargetFragment{};
    };
    APawn* A = MakePlayer(FVector(500.0f, 0.0f, 90.0f));
    Run();
    TestFalse(TEXT("一帧露出不立即确认"), Target.bHasTarget);
    if (!TestTrue(TEXT("前方暴露积累警觉"), Sense.Contacts.Num() == 1 && Sense.Contacts[0].Awareness > 0.0f))
    {
        return false;
    }
    const float FullExposure = Sense.Contacts[0].Awareness;
    Run(5);
    TestTrue(TEXT("持续暴露完成确认"), Target.bHasTarget && Target.bTargetVisible && Target.TargetActor == A);
    const FVector LastPosition = Target.TargetLocation;
    A->SetActorLocation(FVector(-500.0f, 0.0f, 90.0f));
    Run();
    TestTrue(TEXT("遮挡后仅保留最后确认位置"), Target.bHasTarget && !Target.bTargetVisible && Target.TargetLocation == LastPosition);
    Reset();
    Run(15);
    TestFalse(TEXT("背后玩家不自动被发现"), Target.bHasTarget);
    A->SetActorLocation(FVector(50.0f, 0.0f, 90.0f));
    Reset();
    Run(12);
    TestTrue(TEXT("近距离前方玩家不因眼高差落出视野"), Target.bTargetVisible && Target.TargetActor == A);
    A->SetActorLocation(FVector(-50.0f, 0.0f, 90.0f));
    Reset();
    Run(12);
    TestFalse(TEXT("近距离背后仍然严格不可见"), Target.bHasTarget);

    A->SetActorLocation(FVector(500.0f, 0.0f, 90.0f));
    AActor* Wall = World->SpawnActor<AActor>();
    auto* Box = NewObject<UBoxComponent>(Wall);
    Wall->SetRootComponent(Box);
    Box->SetBoxExtent(FVector(10.0f, 250.0f, 160.0f));
    Box->SetCollisionProfileName(TEXT("BlockAll"));
    Box->RegisterComponent();
    Wall->SetActorLocation(FVector(250.0f, 0.0f, 90.0f));
    Reset();
    Run(15);
    TestFalse(TEXT("隔墙不能发现"), Target.bHasTarget);
    Box->SetBoxExtent(FVector(10.0f, 250.0f, 50.0f));
    Wall->SetActorLocation(FVector(250.0f, 0.0f, 70.0f));
    Reset();
    Run();
    TestTrue(TEXT("部分露出也能积累警觉但较慢"), Sense.Contacts.Num() == 1 && Sense.Contacts[0].Awareness > 0.0f && Sense.Contacts[0].Awareness < FullExposure);
    Wall->Destroy();

    A->SetActorLocation(FVector(700.0f, 0.0f, 90.0f));
    APawn* B = MakePlayer(FVector(900.0f, 0.0f, 90.0f));
    Reset();
    Run(12);
    TestTrue(TEXT("首次选择最近确认玩家"), Target.TargetActor == A);
    B->SetActorLocation(FVector(650.0f, 0.0f, 90.0f));
    Run(8);
    TestTrue(TEXT("小幅距离变化不切换"), Target.TargetActor == A);
    B->SetActorLocation(FVector(300.0f, 0.0f, 90.0f));
    Run();
    TestTrue(TEXT("明显占优仍需持续确认"), Target.TargetActor == A);
    Run(5);
    TestTrue(TEXT("持续明显占优后切换"), Target.TargetActor == B);
    B->SetActorLocation(FVector(-900.0f, 0.0f, 90.0f));
    Run();
    TestEqual(TEXT("隐藏目标移动不更新追踪点"), Target.TargetLocation, FVector(300.0f, 0.0f, 90.0f));
    auto& Path = Manager.GetFragmentDataChecked<FBBBMonsterNavigationFragment>(Entity);
    Path.ExcludedPosition = Target.TargetLocation;
    Path.ExcludedUntil = World->GetTimeSeconds() + 4.0f;
    Run();
    TestTrue(TEXT("不可达目标让位给可见候选"), Target.TargetActor == A);
    A->SetCanBeDamaged(false);
    Run();
    TestFalse(TEXT("不可受伤玩家退出索敌"), Target.TargetActor == A);
    B->SetCanBeDamaged(false);
    Run(60);
    TestFalse(TEXT("没有新确认时追踪超时"), Target.bHasTarget);

    Reset();
    FBBBMonsterDamageContribution Hit;
    Hit.PlayerId = 1;
    Hit.Parts.Torso = 1.0;
    Hit.LastHitTime = World->GetTimeSeconds();
    Hit.LastSourcePosition = FVector(-600.0f, 200.0f, 90.0f);
    Hit.bHasSourcePosition = true;
    TArray<uint8> Encoded;
    FMemoryWriter Writer(Encoded);
    FBBBMonsterDamageContribution::StaticStruct()->SerializeItem(Writer, &Hit, nullptr);
    FBBBMonsterDamageContribution Decoded;
    FMemoryReader Reader(Encoded);
    FBBBMonsterDamageContribution::StaticStruct()->SerializeItem(Reader, &Decoded, nullptr);
    TestTrue(TEXT("当前伤害快照序列化保留来源位置"), Decoded == Hit);
    FBBBMonsterDamageLocalControlPacket SourcePacket;
    SourcePacket.Include(Hit);
    auto NewerHit = Hit;
    NewerHit.Parts.Torso = 2.0;
    NewerHit.LastSourcePosition.X += 100.0f;
    SourcePacket.Include(NewerHit);
    SourcePacket.Include(Hit);
    FBBBMonsterDamageFragment Snapshot;
    SourcePacket.Apply(Snapshot);
    TestEqual(TEXT("旧累计快照不能覆盖新命中的来源位置"), Snapshot.Contributions[1].LastSourcePosition, NewerHit.LastSourcePosition);
    Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity).Contributions.Add(1, Hit);
    Run();
    TestTrue(TEXT("来源位置只提供调查线索"), Target.bHasTarget && !Target.bTargetVisible && !Target.TargetActor.IsValid() && Target.TargetLocation == Hit.LastSourcePosition);
    Path.TargetRevision = Target.Revision;
    Path.bReachedDestination = true;
    Run();
    TestFalse(TEXT("到达线索后结束调查且旧伤害不重启"), Target.bHasTarget);
    Path.bReachedDestination = false;
    World->TimeSeconds += 0.11;
    auto& Stimulus = Manager.GetFragmentDataChecked<FBBBMonsterStimulusFragment>(Entity);
    FBBBMonsterSoundLocalControlPacket Sound;
    Sound.Position = FVector(600.0f, 200.0f, 90.0f);
    auto* Inputs = World->GetSubsystem<UBBBMassSubsystem>();
    const auto ConsumeSound = [&Manager, Parser]()
    {
        UE::Mass::FProcessingContext Context(Manager, 0.11f);
        UE::Mass::Executor::Run(*Parser, Context);
    };
    Sound.Time = World->GetTimeSeconds() + 10.0f;
    Inputs->SubmitInput(Entity, Sound);
    ConsumeSound();
    TestEqual(TEXT("未来声音不写入当前线索"), Stimulus.Time, -FLT_MAX);
    Sound.Time = World->GetTimeSeconds() - 5.0f;
    Inputs->SubmitInput(Entity, Sound);
    ConsumeSound();
    TestEqual(TEXT("过期声音不写入当前线索"), Stimulus.Time, -FLT_MAX);
    Sound.Time = World->GetTimeSeconds();
    TestTrue(TEXT("声音预留包合法"), Sound.IsValid() && Sound.CanApply(Stimulus));
    TestTrue(TEXT("预留声音经公开输入入口投递"), Inputs->SubmitInput(Entity, Sound));
    auto& SoundSlot = Manager.GetFragmentDataChecked<FBBBMonsterPerceptionInputFragment>(Entity).Sound;
    TestTrue(TEXT("预留声音只保存一个当前槽位"), SoundSlot.bActive && SoundSlot.Packet.Time == Sound.Time);
    Sound.Position.X += 20.0f;
    TestTrue(TEXT("后到声音覆盖同一输入槽"), Inputs->SubmitInput(Entity, Sound));
    TestEqual(TEXT("没有追加声音事件队列"), SoundSlot.Packet.Position, Sound.Position);
    ConsumeSound();
    TestFalse(TEXT("声音在解析阶段消费一次"), SoundSlot.bActive);
    TestEqual(TEXT("解析应用最新声音位置"), Stimulus.Position, Sound.Position);
    Run();
    TestTrue(TEXT("声音接口只提供位置不确认玩家"), Target.bHasTarget && !Target.bTargetVisible && !Target.TargetActor.IsValid());
    Sound.Time -= 1.0f;
    TestFalse(TEXT("旧声音输入不能覆盖新线索"), Sound.CanApply(Stimulus));

    Manager.DestroyEntity(Entity);
    A->SetCanBeDamaged(true);
    A->SetActorLocation(FVector(500.0f, 0.0f, 90.0f));
    Settings->AllyAlertRange = 1200.0f;
    const auto Source = Create(FVector(0.0f, 0.0f, 90.0f));
    const auto Ally = Create(FVector(-800.0f, 0.0f, 90.0f), 180.0f);
    const auto FarAlly = Create(FVector(-1700.0f, 0.0f, 90.0f), 180.0f);
    Run(12);
    const auto& AllyTarget = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Ally);
    TestTrue(TEXT("同伴收到位置但没有视觉锁定"), AllyTarget.bHasTarget && !AllyTarget.bTargetVisible && !AllyTarget.TargetActor.IsValid());
    TestFalse(TEXT("示警不由接收者继续连锁传播"), Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(FarAlly).bHasTarget);
    Manager.DestroyEntity(Source);
    Manager.DestroyEntity(Ally);
    Manager.DestroyEntity(FarAlly);

    Settings->AllyAlertRange = 0.0f;
    TArray<FMassEntityHandle> Crowd;
    for (int32 Index = 0; Index < 50; ++Index)
    {
        Crowd.Add(Create(FVector(-Index * 5.0f, Index % 5 * 20.0f, 90.0f)));
    }
    const double Started = FPlatformTime::Seconds();
    Run(30);
    const double Elapsed = FPlatformTime::Seconds() - Started;
    int32 ConfirmedCount = 0;
    for (const auto Monster : Crowd)
    {
        const auto& CrowdTarget = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Monster);
        ConfirmedCount += CrowdTarget.bTargetVisible && CrowdTarget.TargetActor == A;
        Manager.DestroyEntity(Monster);
    }
    TestEqual(TEXT("五十只批量感知全部完成确认"), ConfirmedCount, 50);
    AddInfo(FString::Printf(TEXT("[BBBPerception50] Frames=30 Count=50 TotalMs=%.3f MeanMs=%.3f"), Elapsed * 1000.0, Elapsed * 1000.0 / 30.0));
    for (int32 Index = 0; Index < 15; ++Index)
    {
        MakePlayer(FVector(650.0f + Index * 10.0f, (Index - 7) * 20.0f, 90.0f));
    }
    Crowd.Reset();
    for (int32 Index = 0; Index < 50; ++Index)
    {
        Crowd.Add(Create(FVector(-Index * 5.0f, Index % 5 * 20.0f, 90.0f)));
    }
    Run(40);
    int32 SensedCount = 0;
    for (const auto Monster : Crowd)
    {
        const auto& CrowdSense = Manager.GetFragmentDataChecked<FBBBMonsterPerceptionFragment>(Monster);
        const auto& CrowdTarget = Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Monster);
        SensedCount += CrowdSense.Contacts.Num() >= 16 && CrowdTarget.bTargetVisible;
        Manager.DestroyEntity(Monster);
    }
    TestEqual(TEXT("五十只面对十六名玩家时预算轮转不饿死后排实体"), SensedCount, 50);
    const auto Client = Create(FVector(0.0f, 0.0f, 90.0f));
    World->WorldType = EWorldType::PIE;
    World->SetPlayInEditorInitialNetMode(NM_Client);
    TestEqual(TEXT("客机权限回归使用实际网络分支"), World->GetNetMode(), NM_Client);
    Run(12);
    TestFalse(TEXT("客机不独立确认玩家目标"), Manager.GetFragmentDataChecked<FBBBMonsterTargetFragment>(Client).bHasTarget);
    TestTrue(TEXT("客机不生成视觉候选"), Manager.GetFragmentDataChecked<FBBBMonsterPerceptionFragment>(Client).Contacts.IsEmpty());
    Sound.Time = World->GetTimeSeconds();
    TestFalse(TEXT("客机声音不能进入主机决策输入"), Inputs->SubmitInput(Client, Sound));
    Manager.DestroyEntity(Client);
    World->WorldType = EWorldType::Game;
    World->SetPlayInEditorInitialNetMode(NM_Standalone);
    return true;
}

namespace
{
    /** 仅在专用验收关卡主机生成正式配置群体 不保存关卡或资产 */
    FAutoConsoleCommand SpawnPerceptionPIE(
        TEXT("bbb.mass.SpawnPerceptionPIE"), TEXT("仅在僵尸验收关卡主机生成五十只正式配置实体"),
        FConsoleCommandDelegate::CreateLambda([]()
        {
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (!World || World->WorldType != EWorldType::PIE || World->GetNetMode() == NM_Client ||
                    !World->GetPathName().Contains(TEXT("L_BBBZombieMassValidation")))
                {
                    continue;
                }
                const APlayerController* Player = World->GetFirstPlayerController();
                const APawn* Pawn = Player ? Player->GetPawn() : nullptr;
                if (!Pawn)
                {
                    UE_LOG(LogTemp, Error, TEXT("[BBBPerceptionPIE]验收主机尚无玩家"));
                    return;
                }
                TArray<UMassEntityConfigAsset*> Configs;
                for (const TCHAR* Path : {
                    TEXT("/Game/_Project/System/Mass/Monster/Zombie/Male/MEC_BBBZombieMale.MEC_BBBZombieMale"),
                    TEXT("/Game/_Project/System/Mass/Monster/Zombie/Female/MEC_BBBZombieFemale.MEC_BBBZombieFemale")})
                {
                    auto* Config = LoadObject<UMassEntityConfigAsset>(nullptr, Path);
                    if (!Config)
                    {
                        UE_LOG(LogTemp, Error, TEXT("[BBBPerceptionPIE]正式验收配置缺失"));
                        return;
                    }
                    Configs.Add(Config);
                }
                for (TActorIterator<AMassSpawner> It(World); It; ++It)
                {
                    It->DoDespawning();
                }
                UBBBMassValidationLibrary::SpawnPopulation(World, Configs, 50,
                    Pawn->GetActorLocation() + FVector(-1800.0f, 0.0f, 0.0f), 160.0f);
                return;
            }
            UE_LOG(LogTemp, Error, TEXT("[BBBPerceptionPIE]需要专用验收关卡的真实主机 PIE"));
        }));

    /** 只读检查所有真实 PIE 世界的索敌与寻路权责 */
    FAutoConsoleCommand CheckPerceptionPIE(
        TEXT("bbb.mass.CheckPerception"), TEXT("只读检查所有 PIE 世界的目标 视觉候选 路径与主客机权责"),
        FConsoleCommandDelegate::CreateLambda([]()
        {
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (!World || World->WorldType != EWorldType::PIE)
                {
                    continue;
                }
                auto* Subsystem = World->GetSubsystem<UMassEntitySubsystem>();
                if (!Subsystem)
                {
                    continue;
                }
                auto& Manager = Subsystem->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
                Query.AddRequirement<FBBBMonsterPerceptionFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
                int32 Count = 0;
                int32 Targets = 0;
                int32 Visible = 0;
                int32 Contacts = 0;
                int32 Paths = 0;
                int32 Waiting = 0;
                int32 Chasing = 0;
                int32 Moving = 0;
                FMassExecutionContext Context(Manager, 0.0f, false);
                Query.ForEachEntityChunk(Context, [&Count, &Targets, &Visible, &Contacts, &Paths, &Waiting, &Chasing, &Moving](FMassExecutionContext& Chunk)
                {
                    const auto Senses = Chunk.GetFragmentView<FBBBMonsterPerceptionFragment>();
                    const auto Goals = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
                    const auto Routes = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
                    const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
                    const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
                    const auto Velocity = Chunk.GetFragmentView<FMassVelocityFragment>();
                    for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
                    {
                        if (Health[Index].CurrentHealth <= 0.0f)
                        {
                            continue;
                        }
                        ++Count;
                        Targets += Goals[Index].bHasTarget;
                        Visible += Goals[Index].bTargetVisible;
                        Contacts += Senses[Index].Contacts.Num();
                        Paths += Routes[Index].bHasPath;
                        Waiting += Routes[Index].bWaitingForSpace;
                        Chasing += States[Index].State == EBBBMonsterBehavior::Chase;
                        Moving += Velocity[Index].Value.SizeSquared2D() > 100.0f;
                    }
                });
                const bool bAuthorityBoundary = World->GetNetMode() != NM_Client || (Targets == 0 && Contacts == 0 && Paths == 0);
                UE_LOG(LogTemp, Display, TEXT("[BBBPerceptionPIE] World=%d Mode=%d Count=%d Targets=%d Visible=%d Contacts=%d Paths=%d Waiting=%d Chase=%d Moving=%d AuthorityBoundary=%d"),
                    Entry.PIEInstance, World->GetNetMode(), Count, Targets, Visible, Contacts, Paths, Waiting, Chasing, Moving, bAuthorityBoundary);
                ensureMsgf(bAuthorityBoundary, TEXT("[BBBPerceptionPIE]客机不应生成目标或导航决策"));
            }
        }));
}
#endif
