#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/BBBThrowableDefinition.h"
bool UBBBThrowableDefinition::IsValid() const
{
    return Presentation.Mesh && FMath::IsFinite(Launch.Speed) && Launch.Speed > 0.0f
        && !Launch.AngularVelocity.ContainsNaN() && FMath::IsFinite(Launch.VelocityInheritance)
        && Launch.VelocityInheritance >= 0.0f && Physics.Radius > 0.0f && Physics.MassKg > 0.0f
        && Physics.LinearDamping >= 0.0f && Physics.AngularDamping >= 0.0f
        && Explosion.MaxLifetime > 0.0f && Explosion.Radius > 0.0f
        && Explosion.FuseSeconds >= 0.0f && Explosion.FuseSeconds <= Explosion.MaxLifetime
        && (Explosion.bOnImpact || Explosion.FuseSeconds > 0.0f)
        && Explosion.Damage >= 0.0f && Explosion.DurableDamage >= 0.0f
        && Explosion.MinimumDamageRatio >= 0.0f && Explosion.MinimumDamageRatio <= 1.0f
        && Explosion.Stagger >= 0.0f && Explosion.Stagger <= 1.0f
        && Explosion.ImpulseRadius >= 0.0f && Explosion.ImpulseStrength >= 0.0f;
}
