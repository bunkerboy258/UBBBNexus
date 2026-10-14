#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/Processors/BBBThrowableParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
bool FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableLaunchLocalControlPacket Packet)
{
    if (!Packet.IsValid())
    {
        return false;
    }
    Data.Parse.State.Launch = MoveTemp(Packet);
    Data.Parse.State.bLaunch = true;
    return true;
}
bool FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableDetonateLocalControlPacket Packet)
{
    Data.Parse.State.Detonate = Packet;
    Data.Parse.State.bDetonate = true;
    return true;
}
bool FBBBThrowableParseProcessor::Submit(FBBBThrowableRuntimeData& Data, FBBBThrowableExplosionAuthorityFactPacket Packet)
{
    if (!Packet.IsValid())
    {
        return false;
    }
    Data.Parse.State.Fact = MoveTemp(Packet);
    Data.Parse.State.bFact = true;
    return true;
}
void FBBBThrowableParseProcessor::Update(ABBBThrowable& Actor) const
{
    auto& Slots = Actor.RuntimeData.Parse.State;
    auto& Life = Actor.RuntimeData.Lifetime.State;
    auto& Explosion = Actor.RuntimeData.Explosion.State;
    if (Slots.bLaunch && Actor.HasAuthority() && Slots.Launch.CanApply(Life))
    {
        Slots.Launch.Apply(Life);
    }
    if (Slots.bDetonate && Actor.HasAuthority() && Slots.Detonate.CanApply(Explosion))
    {
        Slots.Detonate.Apply(Explosion);
    }
    if (Slots.bFact && Slots.Fact.CanApply(Explosion))
    {
        Slots.Fact.Apply(Explosion);
    }
    Slots = {};
}
