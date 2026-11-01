#include "BBBMonsterSpecialDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Config/BBBSlimeDefinition.h"

bool UBBBMonsterSpecialDefinition::IsValid() const
{
    const float Positive[] = {Cooldown, Windup, Recovery, MaximumRange, RushSpeed, RushDistance,
        KnockdownDuration, StalkRadius, AmbushRange, StalkInterval, FrenzyDuration};
    for (const float Value : Positive)
    {
        if (!FMath::IsFinite(Value) || Value <= 0)
        {
            return false;
        }
    }
    const float Nonnegative[] = {MinimumRange, RushDamage, ImpactSpeed, ImpactLift};
    for (const float Value : Nonnegative)
    {
        if (!FMath::IsFinite(Value) || Value < 0)
        {
            return false;
        }
    }
    return Kind <= EBBBMonsterSpecialKind::Stalker && MaximumRange > MinimumRange
        && KnockdownDuration <= 10 && ImpactSpeed <= 2500 && ImpactLift <= 1200
        && FMath::IsFinite(FrenzyCooldownScale) && FrenzyCooldownScale >= 0.1f && FrenzyCooldownScale <= 1
        && (Kind != EBBBMonsterSpecialKind::Spitter || (Slime && Slime->IsValid()));
}
