#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"

bool UBBBMonsterVariationDefinition::IsValid() const
{
    const auto ValidCycle = [](const FVector& Cycle)
    {
        return !Cycle.ContainsNaN() && Cycle.X > 0.01 && Cycle.Y > 0.01 && Cycle.Z > 0.01;
    };
    if (MovementCycleSeconds.Num() != 3 || !ValidCycle(IdleCycleSeconds) || !FMath::IsFinite(AlertCycleSeconds) || AlertCycleSeconds <= 0.01f)
    {
        return false;
    }
    for (const FVector& Cycle : MovementCycleSeconds)
    {
        if (!ValidCycle(Cycle))
        {
            return false;
        }
    }
    return FMath::IsFinite(LowInfectionRunnerChance) && LowInfectionRunnerChance >= 0.0f && LowInfectionRunnerChance <= 1.0f
        && FMath::IsFinite(HighInfectionRunnerChance) && HighInfectionRunnerChance >= 0.0f && HighInfectionRunnerChance <= LowInfectionRunnerChance
        && FMath::IsFinite(SpeedVariation) && SpeedVariation >= 0.0f && SpeedVariation <= 0.3f
        && FMath::IsFinite(AccelerationVariation) && AccelerationVariation >= 0.0f && AccelerationVariation <= 0.3f
        && FMath::IsFinite(AlertVariation) && AlertVariation >= 0.0f && AlertVariation <= 0.3f;
}
