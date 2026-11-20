#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/BBBThrowableDefinition.h"
bool UBBBThrowableDefinition::IsValid() const
{
    const auto& Area = Explosion.Area;
    return Presentation.Mesh && FMath::IsFinite(Launch.Speed) && Launch.Speed > 0.0f
        && !Launch.AngularVelocity.ContainsNaN() && FMath::IsFinite(Launch.VelocityInheritance)
        && Launch.VelocityInheritance >= 0.0f && Physics.Radius > 0.0f && Physics.MassKg > 0.0f
        && Physics.LinearDamping >= 0.0f && Physics.AngularDamping >= 0.0f
        && Explosion.MaxLifetime > 0.0f && Explosion.Radius > 0.0f
        && Explosion.FuseSeconds >= 0.0f && Explosion.FuseSeconds <= Explosion.MaxLifetime
        && (Explosion.bOnImpact || Explosion.FuseSeconds > 0.0f)
        && Explosion.Damage >= 0.0f && Explosion.DurableDamage >= 0.0f
        && Explosion.MinimumDamageRatio >= 0.0f && Explosion.MinimumDamageRatio <= 1.0f
        && Explosion.Stagger >= 0.0f && FMath::IsFinite(Explosion.FlashSeconds) && Explosion.FlashSeconds >= 0.0f
        && FMath::IsFinite(Area.Duration) && Area.Duration >= 0.0f
        && (Area.Duration == 0.0f || (Presentation.AreaEffect && Presentation.AreaEffectScale > 0.0f
            && FMath::IsFinite(Area.Radius) && Area.Radius > 0.0f && FMath::IsFinite(Area.Height) && Area.Height > 0.0f
            && FMath::IsFinite(Area.Damage) && Area.Damage >= 0.0f && FMath::IsFinite(Area.DamageInterval)
            && Area.DamageInterval >= 0.2f))
        && Explosion.ImpulseRadius >= 0.0f && Explosion.ImpulseStrength >= 0.0f;
}
