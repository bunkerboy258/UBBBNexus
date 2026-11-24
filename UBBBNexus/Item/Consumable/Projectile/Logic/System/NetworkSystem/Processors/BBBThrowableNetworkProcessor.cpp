#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/NetworkSystem/Processors/BBBThrowableNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/BBBThrowableDefinition.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"

namespace
{
/** 依照权威起爆结果向受影响的控制端分发一次视听渐退 */
void PublishFlash(ABBBThrowable& Actor, const FBBBThrowableExplosionFact& Fact)
{
    const auto* Definition = Actor.GetDefinition();
    if (!Definition || Definition->Explosion.FlashSeconds <= 0.0f)
    {
        return;
    }

    const auto& Config = Definition->Explosion;
    UWorld& World = *Actor.GetWorld();
    for (FConstPlayerControllerIterator It = World.GetPlayerControllerIterator(); It; ++It)
    {
        auto* Controller = Cast<ABBBPlayerController>(It->Get());
        const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
        if (!Pawn || !Pawn->CanBeDamaged())
        {
            continue;
        }

        FVector Eye;
        FRotator View;
        Pawn->GetActorEyesViewPoint(Eye, View);
        const float Distance = FVector::Distance(Fact.Position, Eye);
        const float VisualRadius = FMath::Max(Config.Radius * 3.5f, 3500.0f);
        if (Distance >= VisualRadius)
        {
            continue;
        }

        const FVector Direction = (Fact.Position - Eye).GetSafeNormal();
        const float Facing = FMath::Max(0.0, FVector::DotProduct(View.Vector(), Direction));
        constexpr float DirectViewCos = 0.35f;
        if (Distance >= Config.Radius && Facing <= DirectViewCos)
        {
            continue;
        }

        FCollisionQueryParams Occlusion(SCENE_QUERY_STAT(BBBThrowableFlash), false, &Actor);
        Occlusion.AddIgnoredActor(Pawn);
        if (World.LineTraceTestByChannel(Fact.Position, Eye, ECC_Visibility, Occlusion))
        {
            continue;
        }

        float NearStrength = 0.0f;
        if (Distance < Config.Radius)
        {
            const float NearFactor = 1.0f - FMath::Pow(Distance / Config.Radius, 1.5f);
            NearStrength = NearFactor * FMath::Lerp(0.45f, 1.50f, Facing);
        }

        float VisualStrength = 0.0f;
        if (Facing > DirectViewCos)
        {
            const float ViewAlpha = FMath::Clamp((Facing - DirectViewCos) / (1.0f - DirectViewCos), 0.0f, 1.0f);
            const float FarFactor = FMath::Lerp(1.0f, 0.28f, FMath::Clamp(Distance / VisualRadius, 0.0f, 1.0f));
            VisualStrength = FarFactor * FMath::Lerp(0.35f, 1.0f, ViewAlpha);
        }

        const float Strength = FMath::Clamp(FMath::Max(NearStrength, VisualStrength), 0.0f, 1.0f);
        const float Duration = Config.FlashSeconds * FMath::Lerp(0.55f, 1.0f, Strength);
        const float HoldSeconds = Strength >= 0.55f
            ? Duration * FMath::Lerp(0.18f, 0.40f, (Strength - 0.55f) / 0.45f)
            : 0.0f;
        Controller->ClientPlayThrowableFlash(Strength, HoldSeconds, Duration);
    }
}
}

void FBBBThrowableNetworkProcessor::Update(ABBBThrowable& Actor) const
{
    const auto& State = Actor.RuntimeData.Explosion.ReadExplosionState();
    auto& Network = Actor.RuntimeData.Network.State;
    const auto& Life = Actor.RuntimeData.Lifetime.ReadLifetimeState();
    if (Actor.HasAuthority() && !State.Fact.bExploded && Life.Bounce.Serial > Network.PublishedBounceSerial)
    {
        Network.PublishedBounceSerial = Life.Bounce.Serial;
        Actor.Network->MulticastBounce(Life.Bounce);
    }
    if (Actor.HasAuthority() && State.Fact.bExploded && !Network.bPublished)
    {
        Network.bPublished = true;
        Actor.Network->Publish(State.Fact);
        PublishFlash(Actor, State.Fact);
        Actor.ForceNetUpdate();
        const float AreaDuration = State.Fact.bHasArea ? Actor.GetDefinition()->Explosion.Area.Duration : 0.0f;
        Actor.SetLifeSpan(FMath::Max(5.0f, AreaDuration + 2.0f));
    }
}
