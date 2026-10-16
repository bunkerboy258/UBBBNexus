#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/Processors/BBBThrowableParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
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
    Slots = {};
}
