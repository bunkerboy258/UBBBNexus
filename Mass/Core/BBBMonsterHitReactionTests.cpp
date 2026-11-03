#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "MassCommonFragments.h"
#include "MassActorSubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/HitReaction/BBBMonsterHitReactionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"

/** 验证部位碰撞 覆盖输入 死亡屏蔽和方向姿势 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterHitReactionTest, "UBBB.Mass.ZombieHitReaction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterHitReactionTest::RunTest(const FString&)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassActorFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterAvoidanceFragment::StaticStruct(),
        FBBBMonsterHitReactionFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct(),
        FBBBMonsterMobilityFragment::StaticStruct(), FBBBMonsterDamageFragment::StaticStruct(), FBBBMonsterHealthInputFragment::StaticStruct()});
    const auto Entity = Manager.CreateEntity(Type);
    auto* Definition = NewObject<UBBBMonsterDefinition>(World);
    auto& Identity = Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity);
    Identity.Definition = Definition;
    Identity.InstanceId = FGuid::NewGuid();
    const auto Run = [&Manager, World](UClass* Class)
    {
        auto* Processor = NewObject<UMassProcessor>(World, Class);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    Run(UBBBMonsterCollisionProcessor::StaticClass());
    const FVector Points[] = {FVector(0,0,0), FVector(0,0,70), FVector(0,-48,10), FVector(0,48,10), FVector(0,-14,-65), FVector(0,14,-65)};
    for (uint8 Region = 0; Region < 6; ++Region)
    {
        FMassEntityHandle Hit;
        float Time = 1.0f;
        FVector Position;
        FVector Normal;
        EPhysicalSurface Surface;
        uint8 Part = 0;
        const FVector Start = Points[Region] - FVector(100,0,0);
        const FVector End = Points[Region] + FVector(100,0,0);
        TestTrue(TEXT("六部位都可独立命中"), Mass->TraceEntities(Start, End, 1.0f, {}, Hit, Time, Position, Normal, Surface, Part));
        TestEqual(TEXT("返回正确部位"), Part, Region);
        TestTrue(TEXT("命中同一个实体"), Hit == Entity);
    }
    FBBBHitContribution First{1.f, NAME_None, FVector::ZeroVector, FVector::ForwardVector};
    First.Part = 1;
    First.PlayerId = 7;
    First.AddDurableDamage = 1;
    First.Normal = -First.Direction;
    Mass->SubmitInput(Entity, FBBBMonsterDamageLocalControlPacket{{First}});
    First.Part = 5;
    First.Direction = FVector::RightVector;
    Mass->SubmitInput(Entity, FBBBMonsterDamageLocalControlPacket{{First}});
    auto& Slot = Manager.GetFragmentDataChecked<FBBBMonsterHealthInputFragment>(Entity).Damage;
    TestEqual(TEXT("统一输入保留两次独立命中"), Slot.Packet.Hits.Num(), 2);
    Slot.Packet.Apply(Manager.GetFragmentDataChecked<FBBBMonsterDamageFragment>(Entity));
    Slot.bActive = false;
    Run(UBBBMonsterDamageProcessor::StaticClass());
    auto& State = Manager.GetFragmentDataChecked<FBBBMonsterHitReactionFragment>(Entity);
    TestEqual(TEXT("目标发布一次最新反应"), State.Serial, 1u);
    TestTrue(TEXT("目标决定最后受击部位"), State.Region == EBBBMonsterHitRegion::RightLeg);
    Run(UBBBMonsterHitReactionProcessor::StaticClass());
    TestEqual(TEXT("血效编号已消费"), State.PublishedSerial, State.Serial);
    TestTrue(TEXT("批量阶段推进间隔"), FMath::IsNearlyEqual(State.Age, 0.05f));
    Run(UBBBMonsterDamageProcessor::StaticClass());
    TestEqual(TEXT("旧结果不重复驱动身体"), State.Serial, 1u);
    const auto Original = Definition->HitShapes[0].Radius;
    Definition->HitShapes[0].Radius = Original + 8;
    TestTrue(TEXT("命中形状由本目标配置提供"), Definition->HitShapes[0].IsValid());
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth = 0.0f;
    Run(UBBBMonsterCollisionProcessor::StaticClass());
    FMassEntityHandle Hit;
    float Time;
    FVector Position;
    FVector Normal;
    EPhysicalSurface Surface;
    uint8 Part;
    TestFalse(TEXT("死亡立即退出部位碰撞"), Mass->TraceEntities(FVector(-100,0,0), FVector(100,0,0), 1.0f, {}, Hit, Time, Position, Normal, Surface, Part));
    return true;
}

#endif
