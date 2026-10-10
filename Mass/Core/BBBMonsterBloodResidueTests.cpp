#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInterface.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBloodPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"

/** 验证残留形状 方向 环境接收 局部积累与世界预算 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterBloodResidueTest, "UBBB.Mass.ZombieBloodResidue",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterBloodResidueTest::RunTest(const FString& Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true,
        ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("血迹隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };

    auto* Settings = NewObject<UBBBMonsterBloodPresentationDefinition>(World);
    auto* Material = LoadObject<UMaterialInterface>(nullptr,
        TEXT("/Game/_ThirdParty/VFX/RealisticBloodVFX/BloodPack/_Commons/Materials/Decals/M_LiquidDecalBase.M_LiquidDecalBase"));
    if (!TestNotNull(TEXT("真实贴花材质"), Material))
    {
        return false;
    }
    Settings->SplatterMaterials.Add(Material);
    Settings->DropletMaterials.Add(Material);
    Settings->PoolMaterials.Add(Material);
    Settings->MaximumDecals = 32;
    auto* Presentation = World->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>();
    AActor* Floor = World->SpawnActor<AActor>();
    auto* Body = NewObject<UBoxComponent>(Floor);
    Floor->SetRootComponent(Body);
    Body->SetBoxExtent(FVector(5000.0f, 5000.0f, 10.0f));
    Body->SetCollisionProfileName(TEXT("BlockAll"));
    Body->SetCollisionObjectType(ECC_WorldStatic);
    Body->RegisterComponent();
    Floor->SetActorLocation(FVector(0.0f, 0.0f, -10.0f));

    FBBBMonsterBloodDroplet Droplet;
    Droplet.Settings = Settings;
    Droplet.Velocity = FVector(600.0f, 0.0f, -200.0f);
    const auto ContactAt = [World](FVector Point)
    {
        FHitResult Contact;
        World->LineTraceSingleByObjectType(Contact, Point + FVector(0, 0, 10), Point - FVector(0, 0, 10),
            FCollisionObjectQueryParams(ECC_WorldStatic));
        return Contact;
    };
    float FirstSize = 0.0f;
    bool bSizeVaries = false;
    for (int32 Index = 0; Index < 20; ++Index)
    {
        int32 Budget = 8;
        Droplet.Seed = Index + 1;
        Presentation->PlaceResidue(Droplet, ContactAt(FVector(Index * 150.0f, 0.0f, 0.0f)), Budget);
        const auto* Decal = Presentation->Decals.Last().Get();
        TestTrue(TEXT("独立变化的非等宽尺寸"), Decal->DecalSize.Y > Decal->DecalSize.Z * 1.1f);
        TestTrue(TEXT("长轴跟随飞行方向"), FVector::DotProduct(Decal->GetRightVector(), FVector::ForwardVector) > 0.94f);
        TestTrue(TEXT("薄层贴附而非大体积投影"), Decal->DecalSize.X <= 2.0f);
        bSizeVaries |= Index > 0 && !FMath::IsNearlyEqual(FirstSize, Decal->DecalSize.Y);
        FirstSize = Decal->DecalSize.Y;
        TestTrue(TEXT("边缘查询有界"), Budget >= 0);
    }
    TestTrue(TEXT("二十次形状尺寸不锁定为相同印章"), bSizeVaries);

    int32 EdgeBudget = 8;
    const int32 BeforeEdge = Presentation->Decals.Num();
    Presentation->PlaceResidue(Droplet, ContactAt(FVector(4999, 0, 0)), EdgeBudget);
    TestEqual(TEXT("缩小复查仍无完整支撑时不生成悬空血迹"), Presentation->Decals.Num(), BeforeEdge);
    TestTrue(TEXT("边缘复查消耗有界"), EdgeBudget >= 0);

    AActor* Wall = World->SpawnActor<AActor>();
    auto* WallBody = NewObject<UBoxComponent>(Wall);
    Wall->SetRootComponent(WallBody);
    WallBody->SetBoxExtent(FVector(10, 1000, 1000));
    WallBody->SetCollisionProfileName(TEXT("BlockAll"));
    WallBody->SetCollisionObjectType(ECC_WorldStatic);
    WallBody->RegisterComponent();
    Wall->SetActorLocation(FVector(-1000, 0, 1000));
    FHitResult WallContact;
    World->LineTraceSingleByObjectType(WallContact, FVector(-900, 0, 1000), FVector(-1100, 0, 1000),
        FCollisionObjectQueryParams(ECC_WorldStatic));
    Droplet.Velocity = FVector(-600, 200, -100);
    EdgeBudget = 8;
    Presentation->PlaceResidue(Droplet, WallContact, EdgeBudget);
    const auto* WallDecal = Presentation->Decals.Last().Get();
    TestTrue(TEXT("墙面贴花法线与真实接触一致"), FVector::DotProduct(-WallDecal->GetForwardVector(), WallContact.ImpactNormal) > 0.99f);
    TestTrue(TEXT("墙面长轴保持表面切向"), FMath::Abs(FVector::DotProduct(WallDecal->GetRightVector(), WallContact.ImpactNormal)) < 0.01f);
    Droplet.Velocity = FVector(600, 0, -200);

    for (int32 Index = 0; Index < 30; ++Index)
    {
        int32 Budget = 8;
        Droplet.Seed = 100 + Index;
        Presentation->PlaceResidue(Droplet, ContactAt(FVector(0.0f, 300.0f, 0.0f)), Budget);
    }
    int32 LocalCount = 0;
    bool bAccumulated = false;
    for (int32 Index = 0; Index < Presentation->Decals.Num(); ++Index)
    {
        if (FVector::DistSquared(Presentation->Decals[Index]->GetComponentLocation(), FVector(0, 300, 0)) < 100.0f)
        {
            ++LocalCount;
            bAccumulated |= Presentation->Residues[Index].Kind == 2 && Presentation->Residues[Index].Coverage > 0.4f;
        }
    }
    TestEqual(TEXT("局部主血迹达到预算后积累而不无限堆叠"), LocalCount, Settings->LocalDecalLimit);
    TestTrue(TEXT("连续命中增加积血覆盖"), bAccumulated);

    AActor* Step = World->SpawnActor<AActor>();
    auto* StepBody = NewObject<UBoxComponent>(Step);
    Step->SetRootComponent(StepBody);
    StepBody->SetBoxExtent(FVector(200, 200, 5));
    StepBody->SetCollisionProfileName(TEXT("BlockAll"));
    StepBody->SetCollisionObjectType(ECC_WorldStatic);
    StepBody->RegisterComponent();
    Step->SetActorLocation(FVector(0, 300, 25));
    const int32 BeforeStep = Presentation->Decals.Num();
    int32 StepBudget = 8;
    Presentation->PlaceResidue(Droplet, ContactAt(FVector(0, 300, 30)), StepBudget);
    TestEqual(TEXT("不同表面的相近血迹不混合积累"), Presentation->Decals.Num(), BeforeStep + 1);
    TestTrue(TEXT("台阶血迹记录当前接收组件"), Presentation->Residues.Last().Surface.Get() == StepBody);

    const int32 Before = Presentation->Decals.Num();
    Body->bReceivesDecals = false;
    int32 Budget = 8;
    Presentation->PlaceResidue(Droplet, ContactAt(FVector(0, 600, 0)), Budget);
    TestEqual(TEXT("不接收贴花的表面不产生血迹"), Presentation->Decals.Num(), Before);
    Body->bReceivesDecals = true;
    Budget = 0;
    Presentation->PlaceResidue(Droplet, ContactAt(FVector(0, 600, 0)), Budget);
    TestEqual(TEXT("查询预算不足不越界补采样"), Presentation->Decals.Num(), Before);

    for (int32 Index = 0; Index < 100; ++Index)
    {
        Budget = 8;
        Droplet.Seed = 200 + Index;
        Presentation->PlaceResidue(Droplet, ContactAt(FVector(Index * 80.0f - 4000, 1000, 0)), Budget);
    }
    TestEqual(TEXT("世界血迹池硬上限"), Presentation->Decals.Num(), Settings->MaximumDecals);
    TestEqual(TEXT("组件与状态一一对应"), Presentation->Decals.Num(), Presentation->Residues.Num());

    FBBBMonsterBloodImpact Impact;
    Impact.Position = FVector(0, 0, 100);
    for (int32 Index = 0; Index < 100; ++Index)
    {
        Impact.Seed = Index;
        Presentation->EmitDroplets(*Settings, Impact);
    }
    TestEqual(TEXT("仅保存有限当前飞行血滴"), Presentation->Droplets.Num(), Settings->MaximumFlights);
    for (int32 Index = 0; Index < 50; ++Index)
    {
        Presentation->AdvancePresentation(0.05f);
        TestTrue(TEXT("每轮飞行与边缘查询合计不超过上限"), Presentation->LastTraceCount <= Settings->MaximumTracesPerFrame);
    }
    TestEqual(TEXT("飞行血滴碰撞或超时后全部释放"), Presentation->Droplets.Num(), 0);
    for (auto& Residue : Presentation->Residues)
    {
        Residue.CreatedAt = -200.0;
    }
    Presentation->AdvancePresentation(0.05f);
    TestTrue(TEXT("到期组件全部隐藏等待有界复用"), !Presentation->Decals.ContainsByPredicate([](const auto& Decal)
    {
        return Decal->IsVisible();
    }));
    World->GetTimerManager().Tick(125.0f);
    TestTrue(TEXT("视觉淡出不销毁池内组件"), !Presentation->Decals.ContainsByPredicate([](const auto& Decal)
    {
        return !IsValid(Decal.Get()) || !Decal->IsRegistered();
    }));
    Budget = 8;
    Presentation->PlaceResidue(Droplet, ContactAt(FVector(0, 2000, 0)), Budget);
    TestEqual(TEXT("过期池复用不增配组件"), Presentation->Decals.Num(), Settings->MaximumDecals);
    TestTrue(TEXT("过期组件可重新显示"), Presentation->Decals.ContainsByPredicate([](const auto& Decal)
    {
        return Decal->IsVisible();
    }));
    Presentation->Deinitialize();
    TestTrue(TEXT("世界清理不残留组件或飞行状态"), Presentation->Decals.IsEmpty() && Presentation->Residues.IsEmpty() && Presentation->Droplets.IsEmpty());
    return true;
}

#endif
