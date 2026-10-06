#include "BBBWork/UBBBNexus/Mass/Core/BBBMassValidationLibrary.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassEntityConfigAsset.h"
#include "MassEntitySubsystem.h"
#include "MassEntityTemplate.h"
#include "MassSpawnerSubsystem.h"
#include "MassSpawnerTypes.h"
#include "MassSpawnLocationProcessor.h"
#include "SkeletalMeshComponentBudgeted.h"
#include "IAnimationBudgetAllocator.h"
#include "HAL/IConsoleManager.h"
#include "Serialization/JsonSerializer.h"
#include "MassVisualizationTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"

namespace
{
    /** 只允许游戏线程访问当前 PIE 世界 */
    UWorld* ValidationWorld(UObject* Context)
    {
        UWorld* const World = GEngine ? GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::ReturnNull) : nullptr;
        if (!ensureMsgf(IsInGameThread() && World && World->WorldType == EWorldType::PIE, TEXT("[BBBMassValidation]Operation requires the current PIE world")))
        {
            return nullptr;
        }

        return World;
    }
}

UMassEntityConfigAsset* UBBBMassValidationLibrary::CreateActorStressConfig(UObject* WorldContext, UMassEntityConfigAsset* Source)
{
#if WITH_EDITOR
    if (!ValidationWorld(WorldContext) || !Source)
    {
        return nullptr;
    }

    UMassEntityConfigAsset* const Copy = DuplicateObject<UMassEntityConfigAsset>(Source, GetTransientPackage());
    UMassVisualizationTrait* const Visual = Cast<UMassVisualizationTrait>(Copy->GetMutableConfig().FindMutableTrait(UMassVisualizationTrait::StaticClass()));
    if (!ensureMsgf(Visual && Visual->IsIn(Copy), TEXT("[BBBMassValidation]Stress config requires an owned visualization trait")))
    {
        return nullptr;
    }

    for (int32 Index = 0; Index < EMassLOD::Max; ++Index)
    {
        Visual->LODParams.LODMaxCount[Index] = 1000;
        Visual->LODParams.BaseLODDistance[Index] = Index * 100000.0f;
        Visual->LODParams.VisibleLODDistance[Index] = Index * 100000.0f;
    }

    Visual->LODParams.DistanceToFrustum = 100000.0f;
    Visual->Params.LODRepresentation[EMassLOD::High] = EMassRepresentationType::HighResSpawnedActor;
    Visual->Params.LODRepresentation[EMassLOD::Medium] = EMassRepresentationType::HighResSpawnedActor;
    return Copy;
#else
    return nullptr;
#endif
}

TArray<FMassEntityHandle> UBBBMassValidationLibrary::SpawnPopulation(UObject* WorldContext, const TArray<UMassEntityConfigAsset*>& Configs, const int32 Count, const FVector Center, const float Spacing)
{
    TArray<FMassEntityHandle> Result;
    UWorld* const World = ValidationWorld(WorldContext);
    if (!World || !ensureMsgf(Count >= 1 && Count <= 1000 && Configs.Num() >= 1 && Configs.Num() <= 16 && !Center.ContainsNaN() && Spacing >= 100.0f && FMath::IsFinite(Spacing),
        TEXT("[BBBMassValidation]Invalid population configuration")))
    {
        return Result;
    }

    for (UMassEntityConfigAsset* Config : Configs)
    {
        if (!ensureMsgf(Config, TEXT("[BBBMassValidation]Missing entity config")))
        {
            return Result;
        }
    }

    UMassSpawnerSubsystem* const Spawner = World->GetSubsystem<UMassSpawnerSubsystem>();
    if (!ensureMsgf(Spawner, TEXT("[BBBMassValidation]Missing spawner subsystem")))
    {
        return Result;
    }

    const int32 Columns = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(Count)));
    for (int32 Group = 0; Group < Configs.Num(); ++Group)
    {
        FMassTransformsSpawnData Data;
        Data.bRandomize = false;
        for (int32 Index = Group; Index < Count; Index += Configs.Num())
        {
            const FVector Position = Center + FVector((Index / Columns - Columns * 0.5f) * Spacing, (Index % Columns - Columns * 0.5f) * Spacing, 0.0f);
            Data.Transforms.Add(FTransform(Position));
        }

        if (Data.Transforms.IsEmpty())
        {
            continue;
        }

        const FMassEntityTemplate& Template = Configs[Group]->GetOrCreateEntityTemplate(*World);
        TArray<FMassEntityHandle> Spawned;
        Spawner->SpawnEntities(Template.GetTemplateID(), Data.Transforms.Num(), FConstStructView::Make(Data), UMassSpawnLocationProcessor::StaticClass(), Spawned);
        Result.Append(Spawned);
    }

    ensureMsgf(Result.Num() == Count, TEXT("[BBBMassValidation]Population mismatch requested=%d created=%d"), Count, Result.Num());
    UE_LOG(LogTemp, Display, TEXT("[BBBMassValidation]Spawned %d real Mass entities"), Result.Num());
    return Result;
}

FString UBBBMassValidationLibrary::InspectPopulation(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities)
{
    UWorld* const World = ValidationWorld(WorldContext);
    UMassEntitySubsystem* const Subsystem = World ? World->GetSubsystem<UMassEntitySubsystem>() : nullptr;
    if (!Subsystem)
    {
        return TEXT("{\"error\":\"PIE entity subsystem unavailable\"}");
    }

    FMassEntityManager& Manager = Subsystem->GetMutableEntityManager();
    int32 Valid = 0;
    int32 Moving = 0;
    int32 Actors = 0;
    int32 BudgetMeshes = 0;
    int32 Rendered = 0;
    FVector Sum = FVector::ZeroVector;
    TArray<TSharedPtr<FJsonValue>> Positions;
    TArray<TSharedPtr<FJsonValue>> Locomotion;
    for (const FMassEntityHandle Entity : Entities)
    {
        if (!Manager.IsEntityValid(Entity))
        {
            continue;
        }

        ++Valid;
        const FTransformFragment* const Transform = Manager.GetFragmentDataPtr<FTransformFragment>(Entity);
        const FMassVelocityFragment* const Velocity = Manager.GetFragmentDataPtr<FMassVelocityFragment>(Entity);
        const FMassActorFragment* const ActorFragment = Manager.GetFragmentDataPtr<FMassActorFragment>(Entity);
        const FBBBMonsterBehaviorFragment* const Behavior = Manager.GetFragmentDataPtr<FBBBMonsterBehaviorFragment>(Entity);
        const FBBBMonsterMovementFragment* const Movement = Manager.GetFragmentDataPtr<FBBBMonsterMovementFragment>(Entity);
        const FBBBMonsterNavigationFragment* const Navigation = Manager.GetFragmentDataPtr<FBBBMonsterNavigationFragment>(Entity);
        const FBBBMonsterGroundFragment* const Ground = Manager.GetFragmentDataPtr<FBBBMonsterGroundFragment>(Entity);
        if (Behavior && Movement && Navigation)
        {
            TSharedRef<FJsonObject> Sample = MakeShared<FJsonObject>();
            Sample->SetNumberField(TEXT("index"), Entity.Index);
            Sample->SetNumberField(TEXT("serial"), Entity.SerialNumber);
            Sample->SetNumberField(TEXT("behavior"), static_cast<int32>(Behavior->State));
            Sample->SetNumberField(TEXT("gait"), static_cast<int32>(Movement->Gait));
            Sample->SetNumberField(TEXT("actionId"), Behavior->ActionId);
            Sample->SetNumberField(TEXT("enteredAt"), Behavior->StateEnteredTime);
            Sample->SetNumberField(TEXT("endsAt"), Behavior->StateEndsAtTime);
            Sample->SetNumberField(TEXT("speedCmS"), Velocity ? Velocity->Value.Size2D() : 0.0);
            Sample->SetNumberField(TEXT("verticalSpeedCmS"), Velocity ? Velocity->Value.Z : 0.0);
            Sample->SetBoolField(TEXT("grounded"), Ground && Ground->bGrounded);
            Sample->SetNumberField(TEXT("supportNormalZ"), Ground ? Ground->SupportNormal.Z : 0.0);
            Sample->SetBoolField(TEXT("hasPath"), Navigation->bHasPath);
            Sample->SetBoolField(TEXT("reached"), Navigation->bReachedDestination);
            Sample->SetNumberField(TEXT("pathPoints"), Navigation->PathPoints.Num());
            Locomotion.Add(MakeShared<FJsonValueObject>(Sample));
        }
        if (Transform)
        {
            const FVector Position = Transform->GetTransform().GetLocation();
            Sum += Position;
            TArray<TSharedPtr<FJsonValue>> Coordinates;
            Coordinates.Add(MakeShared<FJsonValueNumber>(Position.X));
            Coordinates.Add(MakeShared<FJsonValueNumber>(Position.Y));
            Coordinates.Add(MakeShared<FJsonValueNumber>(Position.Z));
            Positions.Add(MakeShared<FJsonValueArray>(Coordinates));
        }

        Moving += Velocity && Velocity->Value.SizeSquared() > 1.0f ? 1 : 0;
        const AActor* const Actor = ActorFragment ? ActorFragment->Get() : nullptr;
        if (Actor)
        {
            ++Actors;
            USkeletalMeshComponentBudgeted* const Mesh = Actor->FindComponentByClass<USkeletalMeshComponentBudgeted>();
            BudgetMeshes += Mesh ? 1 : 0;
            Rendered += Mesh && Mesh->WasRecentlyRendered(0.2f) ? 1 : 0;
        }
    }

    TSharedRef<FJsonObject> Report = MakeShared<FJsonObject>();
    Report->SetNumberField(TEXT("requested"), Entities.Num());
    Report->SetNumberField(TEXT("validEntities"), Valid);
    Report->SetNumberField(TEXT("movingEntities"), Moving);
    Report->SetNumberField(TEXT("presentationActors"), Actors);
    Report->SetNumberField(TEXT("budgetMeshes"), BudgetMeshes);
    Report->SetNumberField(TEXT("recentlyRenderedMeshes"), Rendered);
    Report->SetNumberField(TEXT("worldDeltaMs"), World->GetDeltaSeconds() * 1000.0);
    Report->SetArrayField(TEXT("positionsCm"), Positions);
    Report->SetArrayField(TEXT("locomotion"), Locomotion);
    Report->SetNumberField(TEXT("worldSeconds"), World->GetTimeSeconds());
    IAnimationBudgetAllocator* const Budget = IAnimationBudgetAllocator::Get(World);
    Report->SetBoolField(TEXT("worldBudgetEnabled"), Budget && Budget->GetEnabled());
    const IConsoleVariable* const Enabled = IConsoleManager::Get().FindConsoleVariable(TEXT("a.Budget.Enabled"));
    const IConsoleVariable* const BudgetMs = IConsoleManager::Get().FindConsoleVariable(TEXT("a.Budget.BudgetMs"));
    Report->SetNumberField(TEXT("globalBudgetEnabled"), Enabled ? Enabled->GetInt() : -1);
    Report->SetNumberField(TEXT("animationBudgetMs"), BudgetMs ? BudgetMs->GetFloat() : -1);
    FString Json;
    FJsonSerializer::Serialize(Report, TJsonWriterFactory<>::Create(&Json));
    return Json;
}

int32 UBBBMassValidationLibrary::DestroyPopulation(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities)
{
    UWorld* const World = ValidationWorld(WorldContext);
    UMassSpawnerSubsystem* const Spawner = World ? World->GetSubsystem<UMassSpawnerSubsystem>() : nullptr;
    if (!Spawner)
    {
        return 0;
    }

    TArray<FMassEntityHandle> Valid;
    for (const FMassEntityHandle Entity : Entities)
    {
        if (Spawner->GetEntityManagerChecked().IsEntityValid(Entity))
        {
            Valid.Add(Entity);
        }
    }

    Spawner->DestroyEntities(Valid);
    UE_LOG(LogTemp, Display, TEXT("[BBBMassValidation]Destroyed %d test entities"), Valid.Num());
    return Valid.Num();
}
