#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassCommonFragments.h"
#include "MassActorSubsystem.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/HitReaction/BBBMonsterHitReactionProcessor.h"

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
    };
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto& Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(), FMassActorFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterAvoidanceFragment::StaticStruct(),
        FBBBMonsterHitReactionFragment::StaticStruct(), FBBBMonsterHitReactionInputFragment::StaticStruct()});
    const auto Entity = Manager.CreateEntity(Type);
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
    FBBBMonsterHitReactionLocalControlPacket First;
    First.Region = EBBBMonsterHitRegion::Head;
    Mass->SubmitInput(Entity, First);
    First.Region = EBBBMonsterHitRegion::RightLeg;
    First.Direction = FVector::RightVector;
    Mass->SubmitInput(Entity, First);
    const auto& Slot = Manager.GetFragmentDataChecked<FBBBMonsterHitReactionInputFragment>(Entity).Hit;
    TestTrue(TEXT("同种输入最后覆盖前者"), Slot.bActive && Slot.Packet.Region == EBBBMonsterHitRegion::RightLeg);
    auto& State = Manager.GetFragmentDataChecked<FBBBMonsterHitReactionFragment>(Entity);
    const auto& Health = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity);
    TestTrue(TEXT("活体接收表现事实"), Slot.Packet.IsValid() && Slot.Packet.CanApply(Health));
    Slot.Packet.Apply(State);
    Run(UBBBMonsterHitReactionProcessor::StaticClass());
    TestEqual(TEXT("命中编号只推进一次"), State.Serial, 1u);
    TestEqual(TEXT("血效编号已消费"), State.PublishedSerial, State.Serial);
    TestTrue(TEXT("批量阶段推进间隔"), FMath::IsNearlyEqual(State.Age, 0.05f));
    Run(UBBBMonsterHitReactionProcessor::StaticClass());
    TestEqual(TEXT("不重放已消费命中"), State.PublishedSerial, 1u);
    Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth = 0.0f;
    TestFalse(TEXT("死亡目标拒绝迟到表现"), Slot.Packet.CanApply(Health));
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
