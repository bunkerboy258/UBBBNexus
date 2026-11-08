#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/Processors/BBBThrowableParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"

void FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableHitLocalControlPacket Packet)
{
    Data.Parse.State.Hit.Hits.Append(Packet.Hits);
    Data.Parse.State.bHit = true;
}

void FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableLaunchLocalControlPacket Packet)
{
    Data.Parse.State.Launch = MoveTemp(Packet);
    Data.Parse.State.bLaunch = true;

}
void FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableDetonateLocalControlPacket Packet)
{
    Data.Parse.State.Detonate = Packet;
    Data.Parse.State.bDetonate = true;

}
void FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableExplosionAuthorityFactPacket Packet)
{
    Data.Parse.State.Fact = MoveTemp(Packet);
    Data.Parse.State.bFact = true;

}
void FBBBThrowableParseProcessor::Update(ABBBThrowable& Actor) const
{
    auto& Slots = Actor.RuntimeData.Parse.State;
    auto& Life = Actor.RuntimeData.Lifetime.State;
    auto& Explosion = Actor.RuntimeData.Explosion.State;
    if (Slots.bHit && Actor.HasAuthority() && !Explosion.Fact.bExploded
        && Slots.Hit.IsValid() && Slots.Hit.CanApply(Life))
    {
        Slots.Hit.Apply(Life);
    }
    if (Slots.bLaunch && Slots.Launch.IsValid() && Actor.HasAuthority() && Slots.Launch.CanApply(Life))
    {
        Slots.Launch.Apply(Life);
    }
    if (Slots.bDetonate && Slots.Detonate.IsValid() && Actor.HasAuthority() && Slots.Detonate.CanApply(Explosion))
    {
        Slots.Detonate.Apply(Explosion);
    }
    if (Slots.bFact && Slots.Fact.IsValid() && Slots.Fact.CanApply(Explosion))
    {
        Slots.Fact.Apply(Explosion);
    }
    if (Slots.bBounce && Actor.HasAuthority() && !Explosion.Fact.bExploded &&
        Slots.Bounce.IsValid() && Slots.Bounce.CanApply(Life))
    {
        Slots.Bounce.Apply(Life);
    }
    if (Slots.bBounceFact && !Actor.HasAuthority() && !Explosion.Fact.bExploded &&
        Slots.BounceFact.IsValid() && Slots.BounceFact.CanApply(Life))
    {
        Slots.BounceFact.Apply(Life);
    }
    Slots = {};
}

void FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableBounceLocalControlPacket Packet)
{
    auto& Slots = Data.Parse.State;
    Slots.Bounce.Contacts.Append(MoveTemp(Packet.Contacts));
    Slots.bBounce = true;
}

void FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableBounceAuthorityFactPacket Packet)
{
    auto& Slots = Data.Parse.State;
    Slots.BounceFact.Facts.Append(MoveTemp(Packet.Facts));
    Slots.bBounceFact = true;
}
