#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/Processors/BBBThrowableExplosionProcessor.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBodyPartDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileImpact.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

namespace
{
    constexpr int32 MaxVisualFragments = 16;

    void BuildFragmentDirections(UWorld& World, const AActor& Actor, const FVector& Origin, float BodyRadius,
        int32 FragmentCount, uint32 Seed, FQuat& OutRotation, TArray<FVector>& OutLocalDirections)
    {
        FHitResult Ground;
        const FCollisionQueryParams GroundParams(SCENE_QUERY_STAT(BBBThrowableFragmentGround), false, &Actor);
        const FCollisionObjectQueryParams StaticWorld(ECC_WorldStatic);
        const bool bGrounded = World.LineTraceSingleByObjectType(Ground, Origin,
            Origin - FVector::UpVector * (BodyRadius * 1.5f + 2.0f), StaticWorld, GroundParams)
            && Ground.ImpactNormal.Z >= 0.45f;
        OutRotation = bGrounded
            ? FQuat::FindBetweenNormals(FVector::UpVector, Ground.ImpactNormal)
            : FQuat::Identity;

        // 空中覆盖完整球面 贴地时将等面积采样限制在支撑面外侧。
        FRandomStream Random(Seed);
        const double StartAngle = Random.FRand() * UE_TWO_PI;
        constexpr double GoldenAngle = 2.399963229728653;
        OutLocalDirections.Reset(FragmentCount);
        for (int32 Index = 0; Index < FragmentCount; ++Index)
        {
            const double HeightFraction = (Index + 0.5) / FragmentCount;
            const double Z = 1.0 - HeightFraction * (bGrounded ? 1.0 : 2.0);
            const double XY = FMath::Sqrt(1.0 - Z * Z);
            const double Angle = StartAngle + Index * GoldenAngle;
            OutLocalDirections.Emplace(XY * FMath::Cos(Angle), XY * FMath::Sin(Angle), Z);
        }
    }

    /** 仅对静态环境采样有限破片火花 避免尸群爆开时数百次穿透射线与 Mass 实体尖峰 */
    void PresentFragmentImpacts(UWorld& World, ABBBThrowable& Actor, const UBBBThrowableDefinition& Definition,
        const FBBBThrowableExplosionFact& Fact)
    {
        const auto& Config = Definition.Explosion;
        if (Config.FragmentCount <= 0 || !Config.FragmentImpactChannel)
        {
            return;
        }

        const int32 SampleCount = FMath::Min(Config.FragmentCount, MaxVisualFragments);
        FQuat Rotation = FQuat::Identity;
        TArray<FVector> LocalDirections;
        BuildFragmentDirections(World, Actor, Fact.Position, Definition.Physics.Radius, SampleCount,
            GetTypeHash(Fact.Position), Rotation, LocalDirections);

        const FCollisionObjectQueryParams StaticWorld(ECC_WorldStatic);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBThrowableFragmentPresentation), false, &Actor);
        Params.bReturnPhysicalMaterial = true;

        TArray<FBBBProjectileImpact> Impacts;
        Impacts.Reserve(SampleCount);
        for (const FVector& LocalDirection : LocalDirections)
        {
            const FVector Direction = Rotation.RotateVector(LocalDirection).GetSafeNormal();
            FHitResult WorldHit;
            if (!World.LineTraceSingleByObjectType(WorldHit, Fact.Position, Fact.Position + Direction * Config.Radius,
                StaticWorld, Params))
            {
                continue;
            }
            FBBBProjectileImpact Impact;
            Impact.Position = WorldHit.ImpactPoint;
            Impact.Normal = WorldHit.ImpactNormal;
            Impact.Surface = SurfaceType1;
            Impacts.Add(Impact);
        }

        if (!Impacts.IsEmpty())
        {
            UNiagaraDataChannelLibrary::GetNiagaraDataChannel(&World, Config.FragmentImpactChannel);
            FBBBProjectilePresentation::PublishImpacts(World, *Config.FragmentImpactChannel, Impacts);
        }
    }
}

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
        Fact.PlayerId = Actor.RuntimeData.Lifetime.ReadLifetimeState().PlayerId;
        Fact.Time = World.GetGameState() ? World.GetGameState()->GetServerWorldTimeSeconds() : World.GetTimeSeconds();
        if (Config.Area.Duration > 0.0f)
        {
            FHitResult Ground;
            FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBThrowableAreaGround), false, &Actor);
            const FCollisionObjectQueryParams StaticWorld(ECC_WorldStatic);
            Fact.bHasArea = World.LineTraceSingleByObjectType(Ground, Fact.Position + FVector(0.0, 0.0, 20.0),
                Fact.Position - FVector(0.0, 0.0, 260.0), StaticWorld, Query) && Ground.ImpactNormal.Z >= 0.45;
            if (Fact.bHasArea)
            {
                Fact.AreaGround = Ground.ImpactPoint;
                State.NextAreaDamageTime = Fact.Time + FMath::Min(Config.Area.DamageInterval, 0.15f);
            }
        }
        const FGuid AttackId = FGuid::NewGuid();
        const bool bFragmentation = Config.FragmentCount > 0;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBThrowableExplosion), false, &Actor);
        Params.AddIgnoredActor(Actor.GetInstigator());
        const FCollisionObjectQueryParams StaticOcclusion(ECC_WorldStatic);
        const bool bAffectMonsters = Config.ImpulseStrength > 0.0f || Config.Damage > 0.0f
            || Config.DurableDamage > 0.0f || Config.Stagger > 0.0f || Config.MonsterShockSeconds > 0.0f;
        if (Mass && bAffectMonsters)
        {
            TArray<FBBBMassCollisionBody> Bodies;
            Mass->OverlapEntities(Fact.Position, FMath::Max(Config.Radius, Config.ImpulseRadius), Bodies);
            TSet<FMassEntityHandle> HitEntities;
            HitEntities.Reserve(Bodies.Num());
            for (const auto& Body : Bodies)
            {
                if (HitEntities.Contains(Body.Entity))
                {
                    continue;
                }
                HitEntities.Add(Body.Entity);
                const FVector Direction = (Body.Center - Fact.Position).GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
                const FVector Surface = Body.Center - Direction * Body.Radius;
                const float Distance = FVector::Distance(Fact.Position, Surface);
                const bool bDamage = Distance <= Config.Radius && (!Config.bCheckOcclusion ||
                    !World.LineTraceTestByObjectType(Fact.Position, Surface, StaticOcclusion, Params));
                const float DistanceAlpha = FMath::Clamp(Distance / FMath::Max(Config.Radius, 1.0f), 0.0f, 1.0f);
                const float Ratio = bDamage ? FMath::Lerp(1.f, Config.MinimumDamageRatio, DistanceAlpha) : 0.f;
                const float FragmentMultiplier = bFragmentation ? FMath::Lerp(3.0f, 1.0f, DistanceAlpha) : 1.0f;
                FBBBHitContribution Hit{Config.Damage * Ratio * FragmentMultiplier, NAME_None, Surface, Direction};
                Hit.AddDurableDamage = Config.DurableDamage * Ratio * FragmentMultiplier;
                Hit.AddStagger = bDamage && Config.MonsterShockSeconds == 0.0f ? Config.Stagger : 0.f;
                Hit.MonsterShockSeconds = bDamage ? Config.MonsterShockSeconds : 0.f;
                Hit.MonsterBurnSeconds = (bDamage && Config.Damage > 0.0f && Config.Area.Damage > 0.0f) ? 5.5f : 0.0f;
                if (Distance < Config.ImpulseRadius && Config.ImpulseStrength > 0.0f)
                {
                    const FVector Force = Direction * Config.ImpulseStrength * (1.f - Distance / Config.ImpulseRadius);
                    if (Config.bVelocityChange)
                    {
                        Hit.AddVelocity = Force;
                    }
                    else
                    {
                        Hit.AddImpulse = Force;
                    }
                }
                Hit.AttackId = AttackId;
                Hit.AttackTime = Fact.Time;
                Hit.PlayerId = Fact.PlayerId;
                Hit.bWorldSource = true;
                Hit.bRadial = true;
                Hit.bHasSourcePosition = true;
                Hit.SourcePosition = Fact.Position;
                Hit.Part = Body.Part;
                Hit.Normal = -Direction;
                if (Hit.IsValid())
                {
                    Mass->SubmitInput(Body.Entity, FBBBMonsterDamageLocalControlPacket{{Hit}});
                }
            }
        }
        const bool bAffectActors = Config.ImpulseStrength > 0.0f || Config.Damage > 0.0f;
        if (bAffectActors)
        {
            FCollisionObjectQueryParams DamageObjects;
            DamageObjects.AddObjectTypesToQuery(ECC_Pawn);
            DamageObjects.AddObjectTypesToQuery(ECC_PhysicsBody);
            DamageObjects.AddObjectTypesToQuery(ECC_WorldDynamic);
            FCollisionQueryParams DamageParams(SCENE_QUERY_STAT(BBBThrowableActorDamage), false, &Actor);
            TArray<FOverlapResult> DamageOverlaps;
            World.OverlapMultiByObjectType(DamageOverlaps, Fact.Position, FQuat::Identity, DamageObjects,
                FCollisionShape::MakeSphere(FMath::Max(Config.Radius, Config.ImpulseRadius)), DamageParams);
            TSet<AActor*> Damaged;
            for (const auto& Overlap : DamageOverlaps)
            {
                AActor* Target = Overlap.GetActor();
                if (!Target || Target == &Actor || Damaged.Contains(Target))
                {
                    continue;
                }
                Damaged.Add(Target);
                const bool bPawn = Cast<APawn>(Target) != nullptr;
                const UPrimitiveComponent* Component = Overlap.GetComponent();
                if (!bPawn && !Cast<ABBBThrowable>(Target) && !Cast<IBBBHitReceiver>(Target)
                    && (!Component || !Component->IsSimulatingPhysics()))
                {
                    continue;
                }
                const bool bSelf = Target == Actor.GetInstigator();
                const bool bAllowDamage = bSelf ? Config.bDamageSelf : (!bPawn || Config.bDamagePlayers);
                // 角色按胶囊根位置计距 防止附着装备或表现网格的大边界扩大伤害范围
                FVector Position = Target->GetActorLocation();
                if (!bPawn && Component)
                {
                    FVector Surface;
                    if (Component->GetClosestPointOnCollision(Fact.Position, Surface) >= 0.0f)
                    {
                        Position = Surface;
                    }
                }
                const float Distance = FVector::Distance(Fact.Position, Position);
                const FVector Direction = (Position - Fact.Position).GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
                FBBBHitContribution Hit{0, NAME_None, Position, Direction};
                Hit.AttackId = AttackId;
                Hit.AttackTime = Fact.Time;
                Hit.PlayerId = Fact.PlayerId;
                Hit.bWorldSource = true;
                Hit.bRadial = true;
                Hit.SourcePosition = Fact.Position;
                Hit.bHasSourcePosition = true;
                FCollisionQueryParams Occlusion = DamageParams;
                Occlusion.AddIgnoredActor(Target);
                if (bAllowDamage && Distance <= Config.Radius && (!Config.bCheckOcclusion ||
                    !World.LineTraceTestByObjectType(Fact.Position, Position, StaticOcclusion, Occlusion)))
                {
                    const float DistanceAlpha = FMath::Clamp(Distance / FMath::Max(Config.Radius, 1.0f), 0.0f, 1.0f);
                    const float FragmentMultiplier = bFragmentation ? FMath::Lerp(2.0f, 1.0f, DistanceAlpha) : 1.0f;
                    Hit.AddDamage = Config.Damage * FMath::Lerp(1.f, Config.MinimumDamageRatio, DistanceAlpha) * FragmentMultiplier;
                }
                if (Distance < Config.ImpulseRadius && Config.ImpulseStrength > 0.0f)
                {
                    const FVector Force = Direction * Config.ImpulseStrength * (1.f - Distance / Config.ImpulseRadius);
                    if (Config.bVelocityChange)
                    {
                        Hit.AddVelocity = Force;
                    }
                    else
                    {
                        Hit.AddImpulse = Force;
                    }
                }
                if (Hit.IsValid())
                {
                    BBBHitReceiver::Submit(Target, Hit);
                }
            }
        }
        UE_LOG(LogTemp, Log, TEXT("[BBBThrowable] Exploded Authority=1 Player=%d Position=%s"),
            Fact.PlayerId, *Fact.Position.ToString());
    }
    if (!State.Fact.bExploded)
    {
        return;
    }
    const auto* GameState = World.GetGameState();
    const double Now = GameState ? GameState->GetServerWorldTimeSeconds() : World.GetTimeSeconds();
    if (Actor.BurstEffect && Now - State.Fact.Time >= 4.0)
    {
        if (Actor.BurstEffect->PoolingMethod == ENCPoolMethod::ManualRelease)
        {
            Actor.BurstEffect->DeactivateImmediate();
            Actor.BurstEffect->ReleaseToPool();
        }
        else
        {
            Actor.BurstEffect->DestroyComponent();
        }
        Actor.BurstEffect = nullptr;
    }
    if (!State.bPresented)
    {
        State.bPresented = true;
        Actor.PhysicsBody->SetSimulatePhysics(false);
        Actor.PhysicsBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Actor.Mesh->SetVisibility(false);
        Actor.Trail->Deactivate();
        if (World.GetNetMode() != NM_DedicatedServer && Now - State.Fact.Time < 2.0)
        {
            if (Definition->Presentation.ExplosionEffect)
            {
                FHitResult Ground;
                FCollisionQueryParams SurfaceParams(SCENE_QUERY_STAT(BBBThrowablePresentationSurface), false, &Actor);
                SurfaceParams.AddIgnoredActor(Actor.GetInstigator());
                const bool bGround =
                    World.LineTraceSingleByChannel(Ground, State.Fact.Position + FVector(0.0, 0.0, 15.0),
                                                   State.Fact.Position - FVector(0.0, 0.0, 65.0), ECC_WorldStatic,
                                                   SurfaceParams) &&
                    Ground.ImpactNormal.Z > 0.25;
                Actor.BurstEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    &World, Definition->Presentation.ExplosionEffect, State.Fact.Position, FRotator::ZeroRotator,
                    FVector(Definition->Presentation.EffectScale), false, false, ENCPoolMethod::ManualRelease);
                auto* Effect = Actor.BurstEffect.Get();
                if (Effect)
                {
                    Effect->SetVariableFloat(TEXT("User.GroundContact"), bGround ? 1.0f : 0.0f);
                    Effect->SetVariableVec3(TEXT("User.GroundOffset"),
                                            bGround ? Ground.ImpactPoint - State.Fact.Position : FVector::ZeroVector);
                    Effect->SetVariableVec3(TEXT("User.GroundNormal"),
                                            bGround ? Ground.ImpactNormal : FVector::UpVector);
                    Effect->Activate(true);
                    UE_LOG(LogTemp, Log, TEXT("[BBBThrowable] Presented Ground=%d Position=%s"), bGround,
                           *State.Fact.Position.ToString());
                }
            }
            if (Definition->Presentation.ExplosionSound)
            {
                UGameplayStatics::PlaySoundAtLocation(&World, Definition->Presentation.ExplosionSound,
                                                      State.Fact.Position, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f,
                                                      Definition->Presentation.ExplosionAttenuation);
            }
            PresentFragmentImpacts(World, Actor, *Definition, State.Fact);
        }
    }
}
