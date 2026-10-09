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
#include "BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "NiagaraDataChannelAsset.h"
#include "NiagaraSystem.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassEntityQuery.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "GameFramework/PlayerState.h"

namespace
{
    UWorld* ValidationWorld(UObject* Context);
}

int32 UBBBMassValidationLibrary::DamagePopulation(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities, const float Damage)
{
    UWorld* World = ValidationWorld(WorldContext);
    if (!World || Entities.IsEmpty() || Entities.Num() > 1000 || !FMath::IsFinite(Damage) || Damage <= 0.0f || Damage > 10000.0f)
    {
        return 0;
    }
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto* Controller = World->GetFirstPlayerController();
    auto* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
    auto* EntitySubsystem = World->GetSubsystem<UMassEntitySubsystem>();
    if (!Mass || !Player || Player->GetPlayerId() < 0 || !EntitySubsystem)
    {
        return 0;
    }
    auto& Manager = EntitySubsystem->GetMutableEntityManager();
    int32 Submitted = 0;
    for (const FMassEntityHandle Entity : Entities)
    {
        if (!Manager.IsEntityValid(Entity))
        {
            continue;
        }
        const auto* State = Manager.GetFragmentDataPtr<FBBBMonsterDamageFragment>(Entity);
        if (!State)
        {
            continue;
        }
        const auto* Current = State->Contributions.Find(Player->GetPlayerId());
        FBBBMonsterDamageContribution Contribution = Current ? *Current : FBBBMonsterDamageContribution();
        Contribution.PlayerId = Player->GetPlayerId();
        Contribution.Parts.Torso += Damage;
        Contribution.LastHitTime = World->GetTimeSeconds();
        FBBBMonsterDamageLocalControlPacket Packet;
        Packet.Include(Contribution);
        Submitted += Mass->SubmitInput(Entity, MoveTemp(Packet)) ? 1 : 0;
    }
    return Submitted;
}

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

bool UBBBMassValidationLibrary::SpawnInspectionProjectile(UObject* WorldContext, UBBBProjectileDefinition* Definition, const FVector Start, const FVector End, const float Damage)
{
    UWorld* const World = ValidationWorld(WorldContext);
    if (!World || !ensureMsgf(Definition && Definition->IsValid() && !Start.ContainsNaN() && !End.ContainsNaN()
        && !Start.Equals(End) && FMath::IsFinite(Damage) && Damage >= 0.0f && Damage <= 10000.0f,
        TEXT("[BBBMassValidation]子弹验证配置或坐标无效")))
    {
        return false;
    }

    UBBBMassSubsystem* const Mass = World->GetSubsystem<UBBBMassSubsystem>();
    APlayerController* const Controller = World->GetFirstPlayerController();
    APawn* const Pawn = Controller ? Controller->GetPawn() : nullptr;
    if (!ensureMsgf(Mass && (Damage == 0.0f || (Controller && Pawn)), TEXT("[BBBMassValidation]子弹伤害验证需要本地玩家")))
    {
        return false;
    }

    FBBBProjectileSpawnLocalControlPacket Packet;
    Packet.MuzzleTransform = FTransform((End - Start).ToOrientationQuat(), Start);
    Packet.Speed = Definition->InitialSpeedCmPerSecond;
    Packet.Lifetime = Definition->MaximumLifetimeSeconds;
    Packet.Damage = Damage;
    Packet.DurableDamage = Definition->BaseDamage > 0.0f ? Definition->DurableDamage * Damage / Definition->BaseDamage : 0.0f;
    Packet.Radius = Definition->CollisionRadiusCm;
    Packet.Penetrations = Definition->MaximumPenetrations;
    Packet.PenetrationMultiplier = Definition->PenetrationDamageMultiplier;
    Packet.CollisionChannel = Definition->CollisionChannel;
    Packet.Source = Pawn;
    Packet.Pawn = Pawn;
    Packet.Controller = Controller;
    Packet.Channel = Definition->PresentationChannel.Get();
    Packet.System = Definition->PresentationSystem.Get();
    Packet.ImpactChannel = Definition->ImpactChannel.Get();
    Packet.TracerLengthCm = Definition->TracerLengthCm;
    Packet.TracerWidthCm = Definition->TracerWidthCm;
    Packet.TracerColor = Definition->TracerColor;
    Packet.bCanCauseDamage = Damage > 0.0f;
    if (!ensureMsgf(Packet.IsValid(), TEXT("[BBBMassValidation]子弹出生输入无效")))
    {
        return false;
    }

    const FMassEntityHandle Entity = Mass->CreateEntity(*Definition->EntityConfig);
    const bool bSubmitted = Entity.IsSet() && Mass->SubmitInput(Entity, MoveTemp(Packet));
    ensureMsgf(bSubmitted, TEXT("[BBBMassValidation]子弹出生失败"));
    UE_LOG(LogTemp, Display, TEXT("[BBBMassValidation]子弹提交 实体=%d 伤害=%.1f 成功=%d"), Entity.Index, Damage, bSubmitted);
    return bSubmitted;
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
    if (Entities.IsEmpty())
    {
        TArray<FMassEntityHandle> CurrentEntities;
        FMassEntityQuery Query(Manager.AsShared());
        Query.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
        Query.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
        FMassExecutionContext Context(Manager, 0.0f, false);
        Query.ForEachEntityChunk(Context, [&CurrentEntities](FMassExecutionContext& Chunk)
        {
            CurrentEntities.Append(Chunk.GetEntities());
        });
        if (!CurrentEntities.IsEmpty())
        {
            return InspectPopulation(WorldContext, CurrentEntities);
        }
    }
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
            const auto* Variation = Manager.GetFragmentDataPtr<FBBBMonsterVariationFragment>(Entity);
            const auto* Snapshot = Manager.GetFragmentDataPtr<FBBBMonsterPresentationStateFragment>(Entity);
            const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
            Sample->SetStringField(TEXT("instanceId"), Network ? Network->InstanceId.ToString() : TEXT(""));
            Sample->SetNumberField(TEXT("variationSeed"), Variation ? Variation->Seed : 0);
            Sample->SetNumberField(TEXT("infection"), Variation ? Variation->Infection : 0);
            Sample->SetNumberField(TEXT("locomotionStyle"), Variation ? Variation->LocomotionStyle : 0);
            Sample->SetNumberField(TEXT("speedScale"), Variation ? Variation->SpeedScale : 0);
            Sample->SetNumberField(TEXT("phaseOffset"), Variation ? Variation->PhaseOffset : 0);
            Sample->SetNumberField(TEXT("loopPhase"), Snapshot ? Snapshot->LoopPhase : 0);
            Sample->SetNumberField(TEXT("maxGait"), static_cast<int32>(Movement->MaxGait));
            Sample->SetNumberField(TEXT("walkSpeed"), Movement->WalkSpeed);
            Sample->SetNumberField(TEXT("runSpeed"), Movement->RunSpeed);
            Sample->SetNumberField(TEXT("sprintSpeed"), Movement->SprintSpeed);
            Sample->SetNumberField(TEXT("actionId"), Behavior->ActionId);
            Sample->SetNumberField(TEXT("enteredAt"), Behavior->StateEnteredTime);
            Sample->SetNumberField(TEXT("endsAt"), Behavior->StateEndsAtTime);
            Sample->SetNumberField(TEXT("speedCmS"), Velocity ? Velocity->Value.Size2D() : 0.0);
            Sample->SetNumberField(TEXT("yawDegrees"), Transform ? Transform->GetTransform().Rotator().Yaw : 0.0);
            Sample->SetNumberField(TEXT("velocityX"), Velocity ? Velocity->Value.X : 0.0);
            Sample->SetNumberField(TEXT("velocityY"), Velocity ? Velocity->Value.Y : 0.0);
            const auto* Combat = Manager.GetFragmentDataPtr<FBBBMonsterCombatFragment>(Entity);
            const auto* Avoidance = Manager.GetFragmentDataPtr<FBBBMonsterAvoidanceFragment>(Entity);
            Sample->SetNumberField(TEXT("attackId"), Combat ? Combat->AttackId : 0);
            Sample->SetBoolField(TEXT("attackFinished"), Combat && Combat->bAttackFinished);
            Sample->SetBoolField(TEXT("hitAttempted"), Combat && Combat->bHitAttempted);
            Sample->SetNumberField(TEXT("separationStrength"), Avoidance ? Avoidance->SeparationStrength : 0.0);
            Sample->SetNumberField(TEXT("verticalSpeedCmS"), Velocity ? Velocity->Value.Z : 0.0);
            Sample->SetBoolField(TEXT("grounded"), Ground && Ground->bGrounded);
            Sample->SetNumberField(TEXT("supportNormalZ"), Ground ? Ground->SupportNormal.Z : 0.0);
            Sample->SetBoolField(TEXT("hasPath"), Navigation->bHasPath);
            Sample->SetBoolField(TEXT("reached"), Navigation->bReachedDestination);
            Sample->SetBoolField(TEXT("partialPath"), Navigation->bPartialPath);
            Sample->SetNumberField(TEXT("navigationFailures"), Navigation->FailureCount);
            Sample->SetNumberField(TEXT("excludedUntil"), Navigation->ExcludedUntil);
            Sample->SetNumberField(TEXT("approachSlot"), Navigation->ApproachSlot);
            Sample->SetBoolField(TEXT("waitingForSpace"), Navigation->bWaitingForSpace);
            const auto* Target = Manager.GetFragmentDataPtr<FBBBMonsterTargetFragment>(Entity);
            const auto* Perception = Manager.GetFragmentDataPtr<FBBBMonsterPerceptionFragment>(Entity);
            Sample->SetBoolField(TEXT("hasTarget"), Target && Target->bHasTarget);
            Sample->SetBoolField(TEXT("targetVisible"), Target && Target->bTargetVisible);
            Sample->SetStringField(TEXT("targetActor"), Target ? GetPathNameSafe(Target->TargetActor.Get()) : TEXT(""));
            Sample->SetStringField(TEXT("targetLocation"), Target ? Target->TargetLocation.ToString() : TEXT(""));
            Sample->SetNumberField(TEXT("targetRevision"), Target ? Target->Revision : 0);
            Sample->SetNumberField(TEXT("visualContacts"), Perception ? Perception->Contacts.Num() : 0);
            float Awareness = 0.0f;
            if (Perception)
            {
                for (const auto& Contact : Perception->Contacts)
                {
                    Awareness = FMath::Max(Awareness, Contact.Awareness);
                }
            }
            Sample->SetNumberField(TEXT("awareness"), Awareness);
            Sample->SetNumberField(TEXT("pathPoints"), Navigation->PathPoints.Num());
            Sample->SetNumberField(TEXT("pathPointIndex"), Navigation->PathPointIndex);
            TArray<TSharedPtr<FJsonValue>> PathCoordinates;
            for (const FVector& Point : Navigation->PathPoints)
            {
                PathCoordinates.Add(MakeShared<FJsonValueString>(Point.ToString()));
            }
            Sample->SetArrayField(TEXT("pathCoordinates"), PathCoordinates);
            const auto* Mobility = Manager.GetFragmentDataPtr<FBBBMonsterMobilityFragment>(Entity);
            const auto* Health = Manager.GetFragmentDataPtr<FBBBMonsterHealthFragment>(Entity);
            Sample->SetBoolField(TEXT("crawling"), Mobility && Mobility->bCrawling);
            Sample->SetNumberField(TEXT("crawlStartedAt"), Mobility ? Mobility->CrawlStartedAt : 0.0f);
            Sample->SetNumberField(TEXT("slowMinimumRatio"), Mobility ? Mobility->SlowMinimumRatio : 1.0f);
            Sample->SetNumberField(TEXT("slowEndsAt"), Mobility ? Mobility->SlowEndsAt : 0.0f);
            Sample->SetNumberField(TEXT("speedRatio"), Mobility ? Mobility->GetSpeedRatio(World->GetTimeSeconds()) : 1.0f);
            Sample->SetBoolField(TEXT("staggering"), Mobility && Mobility->IsStaggering(World->GetTimeSeconds()));
            Sample->SetNumberField(TEXT("staggerStartedAt"), Mobility ? Mobility->StaggerStartedAt : 0.0f);
            Sample->SetNumberField(TEXT("staggerEndsAt"), Mobility ? Mobility->StaggerEndsAt : 0.0f);
            Sample->SetNumberField(TEXT("staggerRegion"), Mobility ? static_cast<int32>(Mobility->StaggerRegion) : 0);
            Sample->SetNumberField(TEXT("hitStopEndsAt"), Mobility ? Mobility->HitStopEndsAt : 0.0f);
            Sample->SetNumberField(TEXT("health"), Health ? Health->CurrentHealth : 0.0f);
            Sample->SetNumberField(TEXT("mainHealth"), Health ? Health->MainHealth : 0.0f);
            Sample->SetNumberField(TEXT("destroyedParts"), Health ? Health->DestroyedParts : 0);
            Sample->SetNumberField(TEXT("attackRatio"), Mobility ? Mobility->AttackRatio : 0.0f);
            Sample->SetNumberField(TEXT("suppression"), Mobility ? Mobility->Suppression : 0.0f);
            Sample->SetStringField(TEXT("actorPath"), ActorFragment && ActorFragment->Get() ? ActorFragment->Get()->GetPathName() : TEXT(""));
            const AActor* const DisplayActor = ActorFragment ? ActorFragment->Get() : nullptr;
            const auto* Corpse = DisplayActor ? DisplayActor->FindComponentByClass<UBBBMonsterPresentationComponent>() : nullptr;
            const auto* CorpseMesh = DisplayActor ? DisplayActor->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
            const auto* Death = Manager.GetFragmentDataPtr<FBBBMonsterDeathFragment>(Entity);
            Sample->SetBoolField(TEXT("corpseActive"), Corpse && Corpse->IsCorpseActive());
            Sample->SetBoolField(TEXT("corpseSimulating"), Corpse && Corpse->IsCorpseSimulating());
            Sample->SetNumberField(TEXT("corpseDestroyAt"), Death ? Death->DestroyAtTime : -1.0f);
            Sample->SetNumberField(TEXT("corpsePelvisZ"), CorpseMesh ? CorpseMesh->GetSocketLocation(TEXT("pelvis")).Z : 0.0f);
            Sample->SetBoolField(TEXT("corpseMeshTick"), CorpseMesh && CorpseMesh->IsComponentTickEnabled());
            Sample->SetNumberField(TEXT("actorYawDegrees"), DisplayActor ? DisplayActor->GetActorRotation().Yaw : 0.0);
            Sample->SetNumberField(TEXT("actorPositionErrorCm"), DisplayActor && Transform ?
                FVector::Distance(DisplayActor->GetActorLocation(), Transform->GetTransform().GetLocation()) : 0.0);
            if (Transform && Navigation->PathPoints.IsValidIndex(Navigation->PathPointIndex))
            {
                const FVector Position = Transform->GetTransform().GetLocation();
                const FVector Waypoint = Navigation->PathPoints[Navigation->PathPointIndex];
                const FVector Foot = Position - FVector(0.0f, 0.0f, Ground && Ground->CapsuleHalfHeight > 0.0f ? Ground->CapsuleHalfHeight : Movement->CapsuleHalfHeight);
                const FVector End = Foot + (Waypoint - Position).GetSafeNormal2D() * 10.0f;
                FVector Hit = End;
                const bool bBlocked = UNavigationSystemV1::NavigationRaycast(World, Foot, End, Hit);
                Sample->SetNumberField(TEXT("waypointDistanceCm"), FVector::Dist2D(Position, Waypoint));
                Sample->SetStringField(TEXT("waypoint"), Waypoint.ToString());
                Sample->SetStringField(TEXT("navigationFoot"), Foot.ToString());
                Sample->SetBoolField(TEXT("navigationRayBlocked"), bBlocked);
                Sample->SetNumberField(TEXT("navigationRayTravelCm"), FVector::Dist2D(Foot, Hit));
                auto* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
                FNavLocation Projected;
                const bool bProjected = NavSystem && NavSystem->ProjectPointToNavigation(Foot, Projected, FVector(100.0f, 100.0f, 250.0f));
                Sample->SetBoolField(TEXT("navigationFootProjected"), bProjected);
                if (bProjected)
                {
                    FVector ProjectedHit = Projected.Location + (End - Foot);
                    const bool bProjectedBlocked = UNavigationSystemV1::NavigationRaycast(World, Projected.Location, ProjectedHit, ProjectedHit);
                    Sample->SetStringField(TEXT("projectedFoot"), Projected.Location.ToString());
                    Sample->SetBoolField(TEXT("projectedRayBlocked"), bProjectedBlocked);
                    Sample->SetNumberField(TEXT("projectedRayTravelCm"), FVector::Dist2D(Projected.Location, ProjectedHit));
                    const auto* NavData = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance());
                    if (NavData)
                    {
                        const FVector NavEnd = Projected.Location + (End - Foot);
                        FVector NodeHit;
                        const bool bNodeBlocked = ARecastNavMesh::NavMeshRaycast(NavData, Projected.NodeRef, Projected.Location, NavEnd, NodeHit, NavData->GetDefaultQueryFilter());
                        Sample->SetBoolField(TEXT("explicitNodeRayBlocked"), bNodeBlocked);
                        Sample->SetNumberField(TEXT("explicitNodeRayTravelCm"), FVector::Dist2D(Projected.Location, NodeHit));
                    }
                }
            }
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
