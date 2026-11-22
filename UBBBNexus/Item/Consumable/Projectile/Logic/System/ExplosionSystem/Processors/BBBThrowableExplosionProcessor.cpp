#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/Processors/BBBThrowableExplosionProcessor.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBodyPartDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
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
#include "NiagaraComponent.h"

namespace
{
    void QueueFragments(UBBBMassSubsystem& Mass, ABBBThrowable& Actor, const FBBBThrowableExplosionConfig& Config,
        const FBBBThrowableExplosionFact& Fact, float BodyRadius, const FGuid& AttackId,
        const TSharedPtr<FBBBProjectileWorldBurst>& Burst)
    {
        FBBBProjectileSpawnLocalControlPacket Packet;
        Packet.LaunchTransform.SetLocation(Fact.Position);
        Packet.Speed = 50000.0f;
        Packet.Lifetime = Config.Radius / Packet.Speed;
        Packet.Damage = Config.Damage;
        Packet.DurableDamage = Config.DurableDamage;
        Packet.StaggerValue = Config.Stagger;
        Packet.Radius = 0.0f;
        Packet.Penetrations = 5;
        Packet.PenetrationMultiplier = 0.70f;
        Packet.Source = &Actor;
        Packet.Pawn = Actor.GetInstigator();
        Packet.bDamageInstigator = Config.bDamageSelf;
        Packet.AttackId = AttackId;
        Packet.AttackTime = Fact.Time;
        Packet.bCanCauseDamage = true;
        Packet.WorldBurst = Burst;

        FHitResult Ground;
        const FCollisionQueryParams GroundParams(SCENE_QUERY_STAT(BBBThrowableFragmentGround), false, &Actor);
        const FCollisionObjectQueryParams StaticWorld(ECC_WorldStatic);
        const bool bGrounded = Actor.GetWorld()->LineTraceSingleByObjectType(Ground, Fact.Position,
            Fact.Position - FVector::UpVector * (BodyRadius * 1.5f + 2.0f), StaticWorld, GroundParams)
            && Ground.ImpactNormal.Z >= 0.45f;
        if (bGrounded)
        {
            Packet.LaunchTransform.SetRotation(FQuat::FindBetweenNormals(FVector::UpVector, Ground.ImpactNormal));
        }

        // 空中覆盖完整球面 贴地时将等面积采样限制在支撑面外侧。
        FRandomStream Random(GetTypeHash(AttackId));
        const double StartAngle = Random.FRand() * UE_TWO_PI;
        constexpr double GoldenAngle = 2.399963229728653;
        TArray<FBBBProjectileSpawnLocalControlPacket> Packets;
        Packets.Reserve(Config.FragmentCount);
        for (int32 Index = 0; Index < Config.FragmentCount; ++Index)
        {
            const double HeightFraction = (Index + 0.5) / Config.FragmentCount;
            const double Z = 1.0 - HeightFraction * (bGrounded ? 1.0 : 2.0);
            const double XY = FMath::Sqrt(1.0 - Z * Z);
            const double Angle = StartAngle + Index * GoldenAngle;
            Packet.LocalDirection = FVector(XY * FMath::Cos(Angle), XY * FMath::Sin(Angle), Z);
            Packets.Add(Packet);
        }
        Mass.QueueProjectileSpawnBatch(*Config.FragmentEntityConfig, MoveTemp(Packets));
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
            Fact.bHasArea = World.LineTraceSingleByObjectType(Ground, Fact.Position + FVector(0.0, 0.0, 12.0),
                Fact.Position - FVector(0.0, 0.0, 180.0), StaticWorld, Query) && Ground.ImpactNormal.Z >= 0.45;
            if (Fact.bHasArea)
            {
                Fact.AreaGround = Ground.ImpactPoint;
                State.NextAreaDamageTime = Fact.Time + Config.Area.DamageInterval;
            }
        }
        const FGuid AttackId = FGuid::NewGuid();
        const bool bFragmentation = Config.FragmentCount > 0;
        TSharedPtr<FBBBProjectileWorldBurst> Burst;
        if (bFragmentation && ensure(Mass && Config.FragmentEntityConfig))
        {
            Burst = MakeShared<FBBBProjectileWorldBurst>();
            Burst->PlayerId = Fact.PlayerId;
            Burst->bDamagePlayers = Config.bDamagePlayers;
        }
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBThrowableExplosion), false, &Actor);
        Params.AddIgnoredActor(Actor.GetInstigator());
        if (Mass)
        {
            TArray<FBBBMassCollisionBody> Bodies;
            Mass->OverlapEntities(Fact.Position, FMath::Max(Config.Radius, Config.ImpulseRadius), Bodies);
            for (const auto& Body : Bodies)
            {
                const FVector Direction = (Body.Center - Fact.Position).GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
                const FVector Surface = Body.Center - Direction * Body.Radius;
                const float Distance = FVector::Distance(Fact.Position, Surface);
                const bool bDamage = !bFragmentation && Distance <= Config.Radius && (!Config.bCheckOcclusion ||
                    !World.LineTraceTestByChannel(Fact.Position, Surface, ECC_Visibility, Params));
                const float Ratio = bDamage ? FMath::Lerp(1.f, Config.MinimumDamageRatio, Distance / Config.Radius) : 0.f;
                FBBBHitContribution Hit{Config.Damage * Ratio, NAME_None, Surface, Direction};
                Hit.AddDurableDamage = Config.DurableDamage * Ratio;
                Hit.AddStagger = bDamage && Config.MonsterShockSeconds == 0.0f ? Config.Stagger : 0.f;
                Hit.MonsterShockSeconds = bDamage ? Config.MonsterShockSeconds : 0.f;
                if (Distance < Config.ImpulseRadius)
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
                    if (Burst)
                    {
                        Burst->EntityHits.FindOrAdd(Body.Entity).Add(MoveTemp(Hit));
                    }
                    else
                    {
                        Mass->SubmitInput(Body.Entity, FBBBMonsterDamageLocalControlPacket{{Hit}});
                    }
                }
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
            const bool bSelf = Target == Actor.GetInstigator();
            const bool bAllowDamage = bSelf ? Config.bDamageSelf : (!Cast<APawn>(Target) || Config.bDamagePlayers);
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
            if (!bFragmentation && bAllowDamage && Distance <= Config.Radius && (!Config.bCheckOcclusion ||
                !World.LineTraceTestByChannel(Fact.Position, Position, ECC_Visibility, Occlusion)))
            {
                Hit.AddDamage = Config.Damage * FMath::Lerp(1.f, Config.MinimumDamageRatio, Distance / Config.Radius);
            }
            if (Distance < Config.ImpulseRadius)
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
            if (Burst && Hit.IsValid())
            {
                Burst->ActorHits.FindOrAdd(TWeakObjectPtr<AActor>(Target)).Add(MoveTemp(Hit));
            }
            else if (!Burst)
            {
                BBBHitReceiver::Submit(Target, Hit);
            }
        }
        if (Burst)
        {
            QueueFragments(*Mass, Actor, Config, Fact, Definition->Physics.Radius, AttackId, Burst);
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
        Actor.BurstEffect->DestroyComponent();
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
                    FVector(Definition->Presentation.EffectScale), false, false, ENCPoolMethod::None);
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
        }
    }
}
