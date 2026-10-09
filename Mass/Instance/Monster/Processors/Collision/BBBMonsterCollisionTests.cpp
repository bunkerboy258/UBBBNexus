#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassEntitySubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutor.h"
#include "MassProcessingContext.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

/** 验证粗筛仅选择候选 精确查询始终读取当前实体的六部位 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterCollisionTest, "UBBB.Mass.Monster.Collision",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterCollisionTest::RunTest(const FString&)
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
    const auto Type = Manager.CreateArchetype({FTransformFragment::StaticStruct(),
        FBBBMonsterAvoidanceFragment::StaticStruct(), FBBBMonsterMobilityFragment::StaticStruct(),
        FBBBMonsterHealthFragment::StaticStruct(), FBBBMonsterNetworkFragment::StaticStruct()});
    const auto Create = [&](const FVector& Position)
    {
        const auto Entity = Manager.CreateEntity(Type);
        Manager.GetFragmentDataChecked<FTransformFragment>(Entity).GetMutableTransform().SetLocation(Position);
        Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(Entity).CurrentHealth = 100.0f;
        Manager.GetFragmentDataChecked<FBBBMonsterAvoidanceFragment>(Entity).CollisionRadius = 45.0f;
        Manager.GetFragmentDataChecked<FBBBMonsterNetworkFragment>(Entity).Definition = NewObject<UBBBMonsterDefinition>(World);
        return Entity;
    };
    const auto First = Create(FVector(0, 0, 90));
    const auto Second = Create(FVector(300, 0, 90));
    const auto Publish = [&]()
    {
        auto* Processor = NewObject<UBBBMonsterCollisionProcessor>(World);
        Processor->CallInitialize(World, Manager.AsShared());
        UE::Mass::FProcessingContext Context(Manager, 0.05f);
        UE::Mass::Executor::Run(*Processor, Context);
    };
    FMassEntityHandle HitEntity;
    float HitTime;
    FVector HitPosition, HitNormal;
    EPhysicalSurface Surface;
    uint8 Part = 255;
    const auto Trace = [&](const FVector& Start, const FVector& End, TConstArrayView<FMassEntityHandle> Ignored = {})
    {
        return Mass->TraceEntities(Start, End, 0.0f, Ignored, HitEntity, HitTime, HitPosition, HitNormal, Surface, Part);
    };
    Publish();
    const FVector Centers[] = {FVector(0, 0, 90), FVector(0, 0, 160), FVector(0, -55, 85),
        FVector(0, 55, 85), FVector(0, -14, 10), FVector(0, 14, 10)};
    for (uint8 Index = 0; Index < UE_ARRAY_COUNT(Centers); ++Index)
    {
        TestTrue(TEXT("没有表现 Actor 仍可命中完整逻辑部位"), Trace(Centers[Index] - FVector(100, 0, 0), Centers[Index] + FVector(100, 0, 0)));
        TestEqual(TEXT("部位分类保持一致"), Part, Index);
        TestTrue(TEXT("空间桶重复候选返回同一实体"), HitEntity == First);
        TestTrue(TEXT("命中时间与法线有效"), HitTime >= 0.0f && HitTime <= 1.0f && !HitNormal.IsNearlyZero());
    }
    TestFalse(TEXT("仅进入粗筛球但不接触部位不会误中"), Trace(FVector(-100, 120, 90), FVector(100, 120, 90)));
    TestTrue(TEXT("相邻目标取最近真实部位"), Trace(FVector(-100, 0, 90), FVector(400, 0, 90)) && HitEntity == First);
    const FMassEntityHandle Ignored[] = {First};
    TestTrue(TEXT("穿透忽略首个目标后命中后方目标"), Trace(FVector(-100, 0, 90), FVector(400, 0, 90), Ignored) && HitEntity == Second);
    TArray<FBBBMassCollisionBody> Results;
    Mass->OverlapEntities(FVector(0, 120, 90), 10.0f, Results);
    TestEqual(TEXT("爆炸范围不得使用粗筛球扩大伤害"), Results.Num(), 0);
    Mass->OverlapEntities(FVector(0, -55, 85), 2.0f, Results);
    TestEqual(TEXT("细部位范围查询按实体去重"), Results.Num(), 1);
    if (!Results.IsEmpty())
    {
        TestEqual(TEXT("爆炸候选保存真实最近部位"), Results[0].Part, static_cast<uint8>(EBBBMonsterHitRegion::LeftArm));
        TestFalse(TEXT("返回的是细部位而非粗筛代理"), Results[0].bCompound);
    }
    auto& Health = Manager.GetFragmentDataChecked<FBBBMonsterHealthFragment>(First);
    Health.DestroyedParts |= 1u << static_cast<uint8>(EBBBMonsterHitRegion::LeftArm);
    TestFalse(TEXT("未重建粗筛时已损毁的手臂立即不可命中"), Trace(FVector(-100, -55, 85), FVector(100, -55, 85)));
    auto& Mobility = Manager.GetFragmentDataChecked<FBBBMonsterMobilityFragment>(First);
    Mobility.bCrawling = true;
    Mobility.CrawlStartedAt = World->GetTimeSeconds() - 10.0f;
    TestTrue(TEXT("无表现载体的爬行头部随姿态转换"), Trace(FVector(70, -100, 70), FVector(70, 100, 70)));
    TestEqual(TEXT("爬行头部仍使用头部伤害池"), Part, static_cast<uint8>(EBBBMonsterHitRegion::Head));
    Mobility.bCrawling = false;
    Health.CurrentHealth = 0.0f;
    TestFalse(TEXT("未重建粗筛时死亡目标立即不可命中"), Trace(FVector(-100, 0, 90), FVector(100, 0, 90)));
    Manager.DestroyEntity(First);
    const auto Replacement = Create(FVector(0, 0, 90));
    TestFalse(TEXT("旧粗筛代理不得命中新一代实体"), Trace(FVector(-100, 0, 90), FVector(100, 0, 90)));
    Publish();
    TestTrue(TEXT("新一代实体发布后正常命中"), Trace(FVector(-100, 0, 90), FVector(100, 0, 90)) && HitEntity == Replacement);
    Mass->OverlapEntities(FVector(150, 0, 90), 130.0f, Results);
    TestEqual(TEXT("多目标范围查询在跨桶后仍按实体去重"), Results.Num(), 2);
    return true;
}

#endif
