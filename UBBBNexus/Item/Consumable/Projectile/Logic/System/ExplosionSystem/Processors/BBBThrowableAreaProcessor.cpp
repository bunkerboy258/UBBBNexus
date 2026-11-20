#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/Processors/BBBThrowableAreaProcessor.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/BBBThrowableDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

namespace
{
/** 查询该点是否在区域中 且区域与目标之间没有场景遮挡 */
bool IsCovered(const ABBBThrowable& Source, const FVector& Ground, const FBBBThrowableAreaConfig& Area,
    const double Age, const FVector& Position, const AActor* Target)
{
    const FVector Relative = Position - Ground;
    if (Relative.Z < 0.0 || Relative.Z > Area.Height || Relative.SizeSquared2D() > FMath::Square(Area.RadiusAtAge(Age)))
    {
        return false;
    }

    FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBThrowableAreaDamage), false, &Source);
    if (Target)
    {
        Query.AddIgnoredActor(Target);
    }
    const FCollisionObjectQueryParams StaticWorld(ECC_WorldStatic);
    return !Source.GetWorld()->LineTraceTestByObjectType(Ground + FVector(0.0, 0.0, 20.0), Position,
        StaticWorld, Query);
}

/** 重叠时让最早仍有效的区域负责此目标 */
bool HasEarlierArea(const ABBBThrowable& Actor, const FBBBThrowableExplosionFact& Current, double Now,
    const FVector& Position, const AActor* Target)
{
    for (TActorIterator<ABBBThrowable> It(Actor.GetWorld()); It; ++It)
    {
        const ABBBThrowable* Other = *It;
        const auto* Definition = Other->GetDefinition();
        if (Other == &Actor || !Definition || Definition->Explosion.Area.Damage <= 0.0f)
        {
            continue;
        }
        const auto& Fact = Other->RuntimeData.Explosion.ReadExplosionState().Fact;
        const auto& Area = Definition->Explosion.Area;
        if (!Fact.bHasArea || Now >= Fact.Time + Area.Duration || Fact.Time > Current.Time ||
            (Fact.Time == Current.Time && Other->GetUniqueID() > Actor.GetUniqueID()))
        {
            continue;
        }
        if (IsCovered(*Other, Fact.AreaGround, Area, Now - Fact.Time, Position, Target))
        {
            return true;
        }
    }
    return false;
}
}

void FBBBThrowableAreaProcessor::Update(ABBBThrowable& Actor) const
{
    const auto* Definition = Actor.GetDefinition();
    auto& State = Actor.RuntimeData.Explosion.State;
    const auto& Fact = State.Fact;
    if (!Definition || !Fact.bHasArea)
    {
        return;
    }

    UWorld& World = *Actor.GetWorld();
    const auto* GameState = World.GetGameState();
    const double Now = GameState ? GameState->GetServerWorldTimeSeconds() : World.GetTimeSeconds();
    const auto& Area = Definition->Explosion.Area;
    const double Age = Now - Fact.Time;
    const float CurrentRadius = Area.RadiusAtAge(Age);
    const double Remaining = Fact.Time + Area.Duration - Now;
    if (Remaining <= 0.0)
    {
        if (Actor.AreaEffect)
        {
            Actor.AreaEffect->DestroyComponent();
            Actor.AreaEffect = nullptr;
        }
        return;
    }

    const double FadeTime = Area.bObscuresSight ? 1.8 : 0.7;
    if (World.GetNetMode() != NM_DedicatedServer && Remaining > FadeTime &&
        !Actor.AreaEffect && Definition->Presentation.AreaEffect)
    {
        Actor.AreaEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(&World, Definition->Presentation.AreaEffect,
            Fact.AreaGround, FRotator::ZeroRotator, FVector(Definition->Presentation.AreaEffectScale),
            false, false, ENCPoolMethod::None);
    }
    if (Actor.AreaEffect && Remaining > FadeTime)
    {
        if (Area.bObscuresSight)
        {
            Actor.AreaEffect->SetVariableFloat(TEXT("User.Radius"), CurrentRadius * 0.67f);
        }
        else
        {
            const float Scale = Definition->Presentation.AreaEffectScale;
            const float SpreadScale = Scale * CurrentRadius / Area.Radius;
            Actor.AreaEffect->SetWorldScale3D(FVector(SpreadScale, SpreadScale, Scale));
        }
        if (!Actor.AreaEffect->IsActive())
        {
            Actor.AreaEffect->Activate(true);
        }
    }
    if (Remaining <= FadeTime && Actor.AreaEffect && Actor.AreaEffect->IsActive())
    {
        Actor.AreaEffect->Deactivate();
    }

    if (!Actor.HasAuthority() || Area.Damage <= 0.0f || Now < State.NextAreaDamageTime)
    {
        return;
    }
    State.NextAreaDamageTime = Now + Area.DamageInterval;
    FBBBHitContribution Hit;
    Hit.AddDamage = Area.Damage;
    Hit.AttackId = FGuid::NewGuid();
    Hit.AttackTime = Now;
    Hit.PlayerId = Fact.PlayerId;
    Hit.SourcePosition = Fact.AreaGround;
    Hit.bHasSourcePosition = true;
    Hit.bWorldSource = true;
    Hit.bRadial = true;
    Hit.Direction = FVector::UpVector;

    for (FConstPlayerControllerIterator It = World.GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* Controller = It->Get();
        APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
        if (!Pawn || !Pawn->CanBeDamaged())
        {
            continue;
        }
        const FVector Position = Pawn->GetActorLocation();
        if (IsCovered(Actor, Fact.AreaGround, Area, Age, Position, Pawn) &&
            !HasEarlierArea(Actor, Fact, Now, Position, Pawn))
        {
            Hit.Position = Position;
            BBBHitReceiver::Submit(Pawn, Hit);
        }
    }

    if (auto* Mass = World.GetSubsystem<UBBBMassSubsystem>())
    {
        TArray<FBBBMassCollisionBody> Bodies;
        Mass->OverlapEntities(Fact.AreaGround, FMath::Sqrt(FMath::Square(CurrentRadius) + FMath::Square(Area.Height)), Bodies);
        TSet<FMassEntityHandle> Damaged;
        for (const auto& Body : Bodies)
        {
            if (Damaged.Contains(Body.Entity) || !IsCovered(Actor, Fact.AreaGround, Area, Age, Body.Center, nullptr) ||
                HasEarlierArea(Actor, Fact, Now, Body.Center, nullptr))
            {
                continue;
            }
            Damaged.Add(Body.Entity);
            Hit.Position = Body.Center;
            Hit.Part = Body.Part;
            Mass->SubmitInput(Body.Entity, FBBBMonsterDamageLocalControlPacket{{Hit}});
        }
    }
}
