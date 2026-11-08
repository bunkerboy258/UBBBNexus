#include "BBBSlimeDefinition.h"

bool UBBBSlimeDefinition::IsValid() const
{
    const float Positive[] = {Speed, FlightRadius, Radius, Lifetime, FlightLifetime};
    for (const float Value : Positive)
    {
        if (!FMath::IsFinite(Value) || Value <= 0)
        {
            return false;
        }
    }
    return EntityConfig && FMath::IsFinite(GravityScale) && GravityScale >= 0
        && FMath::IsFinite(DamagePerSecond) && DamagePerSecond >= 0
        && FMath::IsFinite(SpeedScale) && SpeedScale >= 0.1f && SpeedScale <= 1
        && WorldLimit >= 1 && WorldLimit <= 128 && SourceLimit >= 1 && SourceLimit <= 32;
}
