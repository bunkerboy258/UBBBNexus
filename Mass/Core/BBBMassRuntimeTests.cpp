#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Input/BBBMonsterParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Spawn/BBBProjectileParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Movement/BBBProjectileMovementProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"

/** 隔离世界验证覆盖输入 枪口运动 逻辑碰撞与伤害权限 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMassRuntimeTest, "UBBB.Mass.Runtime",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMassRuntimeTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false)
        .CreatePhysicsScene(true)
        .CreateNavigation(false)
        .CreateAISystem(false)
        .ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离世界"), World))
    {
        return false;
    }

    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle MonsterType = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(),
        FBBBMonsterHealthInputFragment::StaticStruct(), FBBBMonsterNetworkInputFragment::StaticStruct(),
        FBBBMonsterNetworkFragment::StaticStruct(), FBBBMonsterBehaviorFragment::StaticStruct()
    });
    const FMassEntityHandle Monster = Manager.CreateEntity(MonsterType);
    const auto Run = [&Manager, World](UClass* Type)
    {
        UMassProcessor* Processor = NewObject<UMassProcessor>(World, Type);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    };

    FBBBMonsterDamageLocalControlPacket Damage;
    Damage.Damage = 10.0f;
    TestTrue(TEXT("首次投递"), Mass->SubmitInput(Monster, Damage));
    Damage.Damage = 15.0f;
    Mass->SubmitInput(Monster, Damage);
    TestEqual(TEXT("投递不直接扣血"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 100.0f);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Run(UBBBMonsterDamageProcessor::StaticClass());
    TestEqual(TEXT("后到覆盖先到"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 85.0f);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Run(UBBBMonsterDamageProcessor::StaticClass());
    TestEqual(TEXT("消费后不重复扣血"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 85.0f);

    FBBBMonsterHealthRemoteMessagePacket Remote;
    Remote.Health = 60.0f;
    Mass->SubmitInput(Monster, Remote);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Run(UBBBMonsterDamageProcessor::StaticClass());
    Mass->SubmitInput(Monster, Remote);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Run(UBBBMonsterDamageProcessor::StaticClass());
    TestEqual(TEXT("远端结果重复合并幂等"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 60.0f);

    const FMassArchetypeHandle ProjectileType = Manager.CreateArchetype({
        FTransformFragment::StaticStruct(), FMassVelocityFragment::StaticStruct(),
        FBBBProjectileMotionFragment::StaticStruct(), FBBBProjectileCollisionFragment::StaticStruct(),
        FBBBProjectileLifetimeFragment::StaticStruct(), FBBBProjectilePresentationFragment::StaticStruct(),
        FBBBProjectileSpawnInputFragment::StaticStruct()
    });
    const auto Shoot = [&](bool bDamage)
    {
        const FMassEntityHandle Projectile = Manager.CreateEntity(ProjectileType);
        FBBBProjectileSpawnLocalControlPacket Spawn;
        Spawn.MuzzleTransform = FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector::ZeroVector);
        Spawn.Speed = 1000.0f;
        Spawn.Damage = 20.0f;
        Spawn.bCanCauseDamage = bDamage;
        Mass->SubmitInput(Projectile, Spawn);
        Run(UBBBProjectileParseProcessor::StaticClass());
        Run(UBBBProjectileMovementProcessor::StaticClass());
        const FVector Location = Manager.GetFragmentDataChecked<FTransformFragment>(Projectile).GetTransform().GetLocation();
        TestTrue(TEXT("沿当前枪口局部 X 前进"), Location.Equals(FVector(0.0f, 50.0f, 0.0f), 0.01f));

        Mass->BeginCollisionFrame();
        FBBBMassCollisionBody Body;
        Body.Entity = Monster;
        Body.Center = FVector(0.0f, 40.0f, 0.0f);
        Body.Radius = 5.0f;
        Mass->AddCollisionBody(Body);
        Run(UBBBProjectileCollisionProcessor::StaticClass());
        TestTrue(TEXT("无表现 Actor 仍可碰撞"), Manager.GetFragmentDataChecked<FBBBProjectileLifetimeFragment>(Projectile).bPendingDestroy);
        Run(UBBBMonsterParseProcessor::StaticClass());
        Run(UBBBMonsterDamageProcessor::StaticClass());
        Manager.DestroyEntity(Projectile);
    };
    Shoot(false);
    TestEqual(TEXT("镜像子弹不造伤"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 60.0f);
    Shoot(true);
    TestEqual(TEXT("本机控制子弹造伤"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 40.0f);

    Remote.Health = 0.0f;
    Mass->SubmitInput(Monster, Remote);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Run(UBBBMonsterDamageProcessor::StaticClass());
    Remote.Health = 100.0f;
    Mass->SubmitInput(Monster, Remote);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Run(UBBBMonsterDamageProcessor::StaticClass());
    TestEqual(TEXT("旧存活结果不能恢复血量"), Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Monster).CurrentHealth, 0.0f);

    FBBBMonsterStateAuthorityFactPacket Fact;
    Fact.InstanceId = FGuid::NewGuid();
    Fact.Revision = 2;
    Fact.Health = 0.0f;
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    Fact.Revision = 1;
    Fact.Transform.SetLocation(FVector(999.0f));
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    TestEqual(TEXT("过期版本不覆盖"), Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Monster).ReceivedRevision, 2u);
    Fact.InstanceId = FGuid::NewGuid();
    Fact.Revision = 3;
    Mass->SubmitInput(Monster, Fact);
    Run(UBBBMonsterParseProcessor::StaticClass());
    TestEqual(TEXT("错误实例身份不覆盖"), Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Monster).ReceivedRevision, 2u);
    Manager.DestroyEntity(Monster);
    TestFalse(TEXT("回收实体拒绝旧输入"), Mass->SubmitInput(Monster, Damage));
    return true;
}


namespace
{
    FAutoConsoleCommand CheckMassPIE(
        TEXT("bbb.mass.CheckPIE"), TEXT("检查所有 PIE 世界的小怪数量与跨机身份"),
        FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            const int32 Expected = Args.IsEmpty() ? 100 : FCString::Atoi(*Args[0]);
            int32 Worlds = 0;
            bool bValid = true;
            TArray<TSet<FGuid>> Identities;
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (World == nullptr || World->WorldType != EWorldType::PIE)
                {
                    continue;
                }

                ++Worlds;
                FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
                FMassExecutionContext Context(Manager, 0.0f);
                TSet<FGuid>& Ids = Identities.AddDefaulted_GetRef();
                int32 Count = 0;
                Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
                {
                    const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
                    const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
                    for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
                    {
                        ++Count;
                        bValid &= Health[Index].CurrentHealth >= 0.0f && Network[Index].InstanceId.IsValid();
                        Ids.Add(Network[Index].InstanceId);
                    }
                });
                UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] World=%s NetMode=%d Monsters=%d"), *World->GetPathName(), int32(World->GetNetMode()), Count);
                bValid &= Count == Expected;
            }

            bValid &= Worlds > 0;
            for (int32 Index = 1; Index < Identities.Num(); ++Index)
            {
                bValid &= Identities[0].Intersect(Identities[Index]).Num() == Identities[0].Num();
            }
            UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] Result=%s Worlds=%d Expected=%d"), bValid ? TEXT("PASS") : TEXT("FAIL"), Worlds, Expected);
        }));

    FAutoConsoleCommand KillMassClientMonster(
        TEXT("bbb.mass.KillClientMonster"), TEXT("仅在 PIE 客机投递一次致死输入 检查房主接受结果"),
        FConsoleCommandDelegate::CreateLambda([]()
        {
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (World == nullptr || World->WorldType != EWorldType::PIE || World->GetNetMode() != NM_Client)
                {
                    continue;
                }

                FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
                FMassExecutionContext Context(Manager, 0.0f);
                FMassEntityHandle Target;
                Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
                {
                    if (!Target.IsSet() && Chunk.GetNumEntities() > 0)
                    {
                        Target = Chunk.GetEntity(0);
                    }
                });
                if (Target.IsSet())
                {
                    FBBBMonsterDamageLocalControlPacket Packet;
                    Packet.Damage = 1000000.0f;
                    World->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Target, MoveTemp(Packet));
                    UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] Client lethal input submitted"));
                    return;
                }
            }
            UE_LOG(LogTemp, Error, TEXT("[BBBMassCheck] No client monster to test"));
        }));

    FAutoConsoleCommand CheckMassProjectiles(
        TEXT("bbb.mass.CheckProjectiles"), TEXT("检查 PIE 正在飞行的子弹与造伤权限"),
        FConsoleCommandDelegate::CreateLambda([]()
        {
            for (const FWorldContext& Entry : GEngine->GetWorldContexts())
            {
                UWorld* World = Entry.World();
                if (World == nullptr || World->WorldType != EWorldType::PIE)
                {
                    continue;
                }

                FMassEntityManager& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
                FMassEntityQuery Query(Manager.AsShared());
                Query.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBProjectileCollisionFragment>(EMassFragmentAccess::ReadOnly);
                Query.AddRequirement<FBBBProjectilePresentationFragment>(EMassFragmentAccess::ReadOnly);
                FMassExecutionContext Context(Manager, 0.0f);
                int32 Count = 0;
                int32 DamageCount = 0;
                int32 ChannelCount = 0;
                Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
                {
                    const auto Collision = Chunk.GetFragmentView<FBBBProjectileCollisionFragment>();
                    const auto Presentation = Chunk.GetFragmentView<FBBBProjectilePresentationFragment>();
                    for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
                    {
                        ++Count;
                        DamageCount += Collision[Index].bCanCauseDamage ? 1 : 0;
                        ChannelCount += Presentation[Index].Channel.IsValid() ? 1 : 0;
                    }
                });
                UE_LOG(LogTemp, Display, TEXT("[BBBMassCheck] World=%s Projectiles=%d DamageAllowed=%d VFXChannel=%d"), *World->GetPathName(), Count, DamageCount, ChannelCount);
            }
        }));
}

#endif
