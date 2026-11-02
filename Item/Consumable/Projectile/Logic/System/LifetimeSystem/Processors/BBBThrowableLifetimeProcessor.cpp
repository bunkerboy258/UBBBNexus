#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/Processors/BBBThrowableLifetimeProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Initialization/BBBThrowableInitializer.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
void FBBBThrowableLifetimeProcessor::Update(ABBBThrowable& Actor) const
{
    auto &Life = Actor.RuntimeData.Lifetime.State;
    auto &Explosion = Actor.RuntimeData.Explosion.State;
    if (Actor.Definition && !Explosion.Fact.bExploded)
    {
        const double Now = Actor.GetWorld()->GetTimeSeconds();
        const auto &Presentation = Actor.Definition->Presentation;
        if (Actor.HasAuthority() && Life.PendingBounceStrength >= 60.0f && Now - Life.LastBounceSoundTime >= 0.08)
        {
            Life.LastBounceSoundTime = Now;
            ++Life.Bounce.Serial;
            Life.Bounce.Position = Life.PendingBouncePosition;
            Life.Bounce.Strength = Life.PendingBounceStrength;
        }
        Life.PendingBounceStrength = 0.0f;
        if (Life.Bounce.Serial > Life.PresentedBounceSerial)
        {
            Life.PresentedBounceSerial = Life.Bounce.Serial;
            if (Actor.GetNetMode() != NM_DedicatedServer && Presentation.BounceSound)
            {
                UGameplayStatics::PlaySoundAtLocation(Actor.GetWorld(), Presentation.BounceSound, Life.Bounce.Position,
                    FRotator::ZeroRotator, FMath::Clamp(Life.Bounce.Strength / 850.0f, 0.25f, 1.0f),
                    FMath::FRandRange(0.96f, 1.04f), 0.0f, Presentation.BounceAttenuation);
            }
        }
        const float SpeedSquared = Actor.PhysicsBody->GetPhysicsLinearVelocity().SizeSquared();
        Life.bTrailHasMoved |= SpeedSquared >= FMath::Square(80.0f);
        if (Actor.Trail->IsActive() && Life.bTrailHasMoved && SpeedSquared < FMath::Square(80.0f))
        {
            Actor.Trail->Deactivate();
        }
    }
    if (Explosion.Fact.bExploded && Actor.Trail->IsActive())
    {
        Actor.Trail->Deactivate();
    }
    if (!Actor.HasAuthority())
    {
        return;
    }
    if (!Life.bLaunched && Life.PendingDefinition)
    {
        Actor.Definition = Life.PendingDefinition;
        Actor.SetActorTransform(Life.PendingTransform);
        FBBBThrowableInitializer{}.Initialize(Actor);
        const auto& Launch = Actor.Definition->Launch;
        Actor.PhysicsBody->SetPhysicsLinearVelocity(Life.PendingTransform.GetUnitAxis(EAxis::X) * Launch.Speed
            + Life.PendingVelocity * Launch.VelocityInheritance);
        Actor.PhysicsBody->SetPhysicsAngularVelocityInDegrees(Life.PendingTransform.TransformVectorNoScale(Launch.AngularVelocity));
        Life.bLaunched = true;
        Life.LaunchTime = Actor.GetWorld()->GetTimeSeconds();
        Actor.ForceNetUpdate();
        UE_LOG(LogTemp, Log, TEXT("[BBBThrowable] Launched Authority=1 Mode=PredictiveInterpolation Position=%s"), *Actor.GetActorLocation().ToString());
    }
    if (!Life.bLaunched || !Actor.Definition || Explosion.Fact.bExploded)
    {
        return;
    }
    const double Age = Actor.GetWorld()->GetTimeSeconds() - Life.LaunchTime;
    const auto& Config = Actor.Definition->Explosion;
    if (Config.FuseSeconds > 0.0f && Age >= Config.FuseSeconds)
    {
        Explosion.bRequested = true;
    }
    if (Age >= Config.MaxLifetime && !Explosion.bRequested)
    {
        Actor.Destroy();
    }
}
