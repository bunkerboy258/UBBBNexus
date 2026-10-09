#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "HitReactProfile.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

/** 验证被配置冷却拒绝的连射不丢掉活动混合 接受后同骨骼仅保留一份 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterHitReactionRefreshTest, "UBBB.Mass.ZombieHitReactionRefresh",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterHitReactionRefreshTest::RunTest(const FString&)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("连射隔离物理世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr,
        TEXT("/Game/_Project/System/Mass/Monster/Zombie/Male/Variants/Michael/BP_BBBZombieMichaelPresentation.BP_BBBZombieMichaelPresentation_C"));
    if (!TestNotNull(TEXT("正式近距离表现类"), Class))
    {
        return false;
    }
    auto* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(Class);
    if (!TestNotNull(TEXT("连射验收载体"), Actor))
    {
        return false;
    }
    auto* Reaction = Actor->FindComponentByClass<UBBBMonsterHitReactionComponent>();
    auto* Profile = NewObject<UHitReactProfile>(World);
    Profile->Cooldown = 100.0f;
    Profile->MaxBlendWeight = 0.6f;
    Profile->SubsequentImpulseScalars.Reset();
    Reaction->Mesh = Actor->GetMonsterMesh();
    Reaction->Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Reaction->bHasInitialized = true;
    Reaction->bProfilesLoaded = true;
    Reaction->AvailableProfiles.Reset();
    Reaction->AvailableProfiles.Add(Profile);
    Reaction->ActiveProfiles.Reset();
    Reaction->ActiveProfiles.Add(Profile);
    Reaction->ToggleHitReactSystem(true, false);
    TestTrue(TEXT("验收前启用插件的全局受击开关"), Reaction->IsHitReactSystemEnabled());
    auto& Blend = Reaction->PhysicsBlends.Add_GetRef({});
    Blend.HitReact(Reaction->Mesh, Profile, TEXT("spine_01"), {}, {});
    Blend.Tick(0.1f);
    FBBBMonsterHitReactionFragment Hit;
    Hit.Serial = 1;
    Hit.Age = 0.0f;
    Hit.Direction = FVector::ForwardVector;
    Hit.Position = Reaction->Mesh->GetSocketLocation(TEXT("spine_01"));
    Reaction->ApplyHitFacts(Hit, true, false);
    TestEqual(TEXT("被冷却拒绝的命中仍消费编号"), Reaction->GetObservedHitSerial(), 1u);
    TestEqual(TEXT("拒绝连射不能清空当前骨骼受力"), Reaction->GetPhysicsBlends().Num(), 1);
    TestTrue(TEXT("拒绝连射不重启当前混合年龄"), FMath::IsNearlyEqual(Reaction->GetPhysicsBlends()[0].PhysicsState.GetElapsedTime(), 0.1f));
    Profile->Cooldown = 0.0f;
    Hit.Serial = 2;
    Reaction->ApplyHitFacts(Hit, true, true);
    TestTrue(TEXT("踉跄姿态仍允许叠加物理冲击"), Reaction->bHasAppliedReaction);
    TestEqual(TEXT("已接受连射只保留一个同骨骼混合"), Reaction->GetPhysicsBlends().Num(), 1);
    TestTrue(TEXT("接受连射刷新当前物理混合"), FMath::IsNearlyZero(Reaction->GetPhysicsBlends()[0].PhysicsState.GetElapsedTime()));
    Reaction->ResetPresentation();
    TestEqual(TEXT("复用清除全部局部受力"), Reaction->GetPhysicsBlends().Num(), 0);
    return true;
}

#endif
