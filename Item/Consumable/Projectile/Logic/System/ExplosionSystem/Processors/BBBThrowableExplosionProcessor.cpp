#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/Processors/BBBThrowableExplosionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBodyPartDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

void FBBBThrowableExplosionProcessor::Update(ABBBThrowable& Actor) const
{
    const auto* Definition = Actor.Definition.Get();
    if (!Definition)
    {
        return;
    }
    UWorld& World = *Actor.GetWorld();
    auto& State = Actor.RuntimeData.Explosion.State;
    const auto& Config = Definition->Explosion;
    UBBBMassSubsystem* Mass = World.GetSubsystem<UBBBMassSubsystem>();
    if (Actor.HasAuthority() && State.bRequested && !State.Fact.bExploded)
    {
        auto& Fact = State.Fact;
        Fact.bExploded = true;
        Fact.Position = Actor.GetActorLocation();
        const auto* Controller = Cast<APlayerController>(Actor.GetOwner());
        const auto* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
        Fact.PlayerId = Player ? Player->GetPlayerId() : INDEX_NONE;
        Fact.Time = World.GetGameState() ? World.GetGameState()->GetServerWorldTimeSeconds() : World.GetTimeSeconds();
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBThrowableExplosion), false, &Actor);
        Params.AddIgnoredActor(Actor.GetInstigator());
        if (Mass)
        {
            TArray<FBBBMassCollisionBody> Bodies;
            Mass->OverlapEntities(Fact.Position, Config.Radius, Bodies);
            for (const auto& Body : Bodies)
            {
                const FVector Direction = (Body.Center - Fact.Position).GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
                const FVector Surface = Body.Center - Direction * Body.Radius;
                if (Config.bCheckOcclusion && World.LineTraceTestByChannel(Fact.Position, Surface, ECC_Visibility, Params))
                {
                    continue;
                }
                FBBBThrowableDamageFact Result;
                if (!Mass->QueryMonsterInstanceId(Body.Entity, Result.InstanceId))
                {
                    continue;
                }
                const float Ratio = FMath::Lerp(1.0f, Config.MinimumDamageRatio,
                    FMath::Clamp(FVector::Distance(Fact.Position, Surface) / Config.Radius, 0.0f, 1.0f));
                const FGuid AttackId = FGuid::NewGuid();
                float GreatestImpact = -1.0f;
                for (uint8 Part = 0; Part <= static_cast<uint8>(EBBBMonsterHitRegion::RightLeg); ++Part)
                {
                    FBBBMonsterBodyPartDefinition PartConfig;
                    if (!Mass->QueryMonsterBodyPart(Body.Entity, Part, PartConfig))
                    {
                        continue;
                    }
                    const auto Region = static_cast<EBBBMonsterHitRegion>(Part);
                    const float Share = Region == EBBBMonsterHitRegion::Torso ? 0.5f : 0.1f;
                    const double Damage = Ratio * Share * (Config.Damage * (1.0 - PartConfig.Durability)
                        + Config.DurableDamage * PartConfig.Durability);
                    Result.Parts.Add(Region, Damage);
                    const float Impact = Damage * PartConfig.StaggerMultiplier;
                    if (Impact > GreatestImpact)
                    {
                        GreatestImpact = Impact;
                        Result.Stagger.ActiveRegion = Region;
                        Result.Stagger.StartedAt = Fact.Time;
                        Result.Stagger.EndsAt = Fact.Time + PartConfig.StaggerDuration;
                    }
                }
                TArray<FBBBMonsterDamageContribution> Previous;
                double AvailableAt = 0.0;
                if (Mass->QueryDamage(Body.Entity, Previous))
                {
                    for (const auto& Value : Previous)
                    {
                        AvailableAt = FMath::Max(AvailableAt, Value.Stagger.EndsAt + FBBBMonsterStaggerContribution::RecoveryDuration);
                    }
                }
                if (GreatestImpact > 0.0f && Fact.Time >= AvailableAt && FMath::FRand() < Config.Stagger)
                {
                    Result.Stagger.ActiveAttackId = AttackId;
                }
                Fact.Targets.Add(MoveTemp(Result));
            }
        }
        FCollisionObjectQueryParams DamageObjects;
        DamageObjects.AddObjectTypesToQuery(ECC_Pawn);
        DamageObjects.AddObjectTypesToQuery(ECC_PhysicsBody);
        DamageObjects.AddObjectTypesToQuery(ECC_WorldDynamic);
        DamageObjects.AddObjectTypesToQuery(ECC_WorldStatic);
        FCollisionQueryParams DamageParams(SCENE_QUERY_STAT(BBBThrowableActorDamage), false, &Actor);
        TArray<FOverlapResult> DamageOverlaps;
        World.OverlapMultiByObjectType(DamageOverlaps, Fact.Position, FQuat::Identity, DamageObjects,
            FCollisionShape::MakeSphere(Config.Radius), DamageParams);
        TSet<AActor*> Damaged;
        for (const auto& Overlap : DamageOverlaps)
        {
            AActor* Target = Overlap.GetActor();
            if (!Target || Target == &Actor || Damaged.Contains(Target))
            {
                continue;
            }
            Damaged.Add(Target);
            const bool bSelf = Target == Actor.GetInstigator();
            if ((bSelf && !Config.bDamageSelf) || (Cast<APawn>(Target) && !bSelf && !Config.bDamagePlayers))
            {
                continue;
            }
            // 角色按胶囊根位置计距 防止附着装备或表现网格的大边界扩大伤害范围
            FVector Position = Target->GetActorLocation();
            if (!Cast<APawn>(Target) && Overlap.GetComponent())
            {
                FVector Surface;
                if (Overlap.GetComponent()->GetClosestPointOnCollision(Fact.Position, Surface) >= 0.0f)
                {
                    Position = Surface;
                }
            }
            const float Distance = FVector::Distance(Fact.Position, Position);
            if (Distance > Config.Radius)
            {
                continue;
            }
            FCollisionQueryParams Occlusion = DamageParams;
            Occlusion.AddIgnoredActor(Target);
            if (Config.bCheckOcclusion && World.LineTraceTestByChannel(Fact.Position, Position, ECC_Visibility, Occlusion))
            {
                continue;
            }
            const float Ratio = FMath::Lerp(1.0f, Config.MinimumDamageRatio,
                FMath::Clamp(Distance / Config.Radius, 0.0f, 1.0f));
            UGameplayStatics::ApplyDamage(Target, Config.Damage * Ratio, Actor.GetInstigatorController(), &Actor, nullptr);
        }
        if (Config.ImpulseRadius > 0.0f && Config.ImpulseStrength > 0.0f)
        {
            FCollisionObjectQueryParams Objects;
            Objects.AddObjectTypesToQuery(ECC_PhysicsBody);
            Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
            TArray<FOverlapResult> Overlaps;
            World.OverlapMultiByObjectType(Overlaps, Fact.Position, FQuat::Identity, Objects,
                FCollisionShape::MakeSphere(Config.ImpulseRadius), Params);
            TSet<UPrimitiveComponent*> Applied;
            for (const auto& Overlap : Overlaps)
            {
                UPrimitiveComponent* Component = Overlap.GetComponent();
                if (Component && Component != Actor.PhysicsBody && Component->IsSimulatingPhysics() && !Applied.Contains(Component))
                {
                    Applied.Add(Component);
                    Component->AddRadialImpulse(Fact.Position, Config.ImpulseRadius, Config.ImpulseStrength,
                        ERadialImpulseFalloff::RIF_Linear, Config.bVelocityChange);
                }
            }
        }
        UE_LOG(LogTemp, Log, TEXT("[BBBThrowable] Exploded Authority=1 Player=%d Targets=%d Position=%s"),
            Fact.PlayerId, Fact.Targets.Num(), *Fact.Position.ToString());
    }
    if (!State.Fact.bExploded)
    {
        return;
    }
    if (!State.bPresented)
    {
        State.bPresented = true;
        Actor.PhysicsBody->SetSimulatePhysics(false);
        Actor.PhysicsBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Actor.Mesh->SetVisibility(false);
        if (World.GetNetMode() != NM_DedicatedServer)
        {
            if (Definition->Presentation.ExplosionEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(&World, Definition->Presentation.ExplosionEffect,
                    State.Fact.Position, FRotator::ZeroRotator, FVector(Definition->Presentation.EffectScale));
            }
            if (Definition->Presentation.ExplosionSound)
            {
                UGameplayStatics::PlaySoundAtLocation(&World, Definition->Presentation.ExplosionSound, State.Fact.Position);
            }
        }
    }
    if (!Mass)
    {
        return;
    }
    bool bLocalSource = false;
    for (FConstPlayerControllerIterator It = World.GetPlayerControllerIterator(); It; ++It)
    {
        const APlayerController* Controller = It->Get();
        const APlayerState* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
        bLocalSource |= Controller && Controller->IsLocalController() && Player && Player->GetPlayerId() == State.Fact.PlayerId;
    }
    if (!bLocalSource)
    {
        return;
    }
    for (TActorIterator<ABBBMassNetworkActor> It(&World); It; ++It)
    {
        for (int32 Index = 0; Index < State.Fact.Targets.Num(); ++Index)
        {
            if (State.Credited.Contains(Index))
            {
                continue;
            }
            const auto& Target = State.Fact.Targets[Index];
            const auto Entity = It->FindEntity(Target.InstanceId);
            FBBBMonsterDamageLocalControlPacket Packet;
            if (!Mass->QueryDamage(Entity, Packet.Contributions))
            {
                continue;
            }
            const auto* Previous = Packet.Contributions.FindByPredicate([&State](const auto& Value)
            {
                return Value.PlayerId == State.Fact.PlayerId;
            });
            FBBBMonsterDamageContribution Value = Previous ? *Previous : FBBBMonsterDamageContribution{};
            Value.PlayerId = State.Fact.PlayerId;
            for (uint8 Part = 0; Part <= static_cast<uint8>(EBBBMonsterHitRegion::RightLeg); ++Part)
            {
                const auto Region = static_cast<EBBBMonsterHitRegion>(Part);
                Value.Parts.Add(Region, Target.Parts.Get(Region));
            }
            Value.LastHitTime = State.Fact.Time;
            Value.LastHitRegion = EBBBMonsterHitRegion::Torso;
            Value.LastSourcePosition = State.Fact.Position;
            Value.bHasSourcePosition = true;
            Value.Stagger.Merge(Target.Stagger);
            Packet.Include(Value);
            if (Mass->SubmitInput(Entity, MoveTemp(Packet)))
            {
                State.Credited.Add(Index);
                UE_LOG(LogTemp, Log, TEXT("[BBBThrowable] DamageCredited Player=%d Target=%s Amount=%f"),
                    Value.PlayerId, *Target.InstanceId.ToString(), Target.Parts.Sum());
            }
        }
        break;
    }
}
