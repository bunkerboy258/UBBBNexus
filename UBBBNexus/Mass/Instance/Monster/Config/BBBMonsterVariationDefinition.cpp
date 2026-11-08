#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"

bool UBBBMonsterVariationDefinition::IsValid() const
{
    return MinInfection <= MaxInfection
        && FMath::IsFinite(LowInfectionRunnerChance) && LowInfectionRunnerChance >= 0.0f && LowInfectionRunnerChance <= 1.0f
        && FMath::IsFinite(HighInfectionRunnerChance) && HighInfectionRunnerChance >= 0.0f && HighInfectionRunnerChance <= LowInfectionRunnerChance
        && FMath::IsFinite(SpeedVariation) && SpeedVariation >= 0.0f && SpeedVariation <= 0.3f
        && FMath::IsFinite(AccelerationVariation) && AccelerationVariation >= 0.0f && AccelerationVariation <= 0.3f
        && FMath::IsFinite(AlertVariation) && AlertVariation >= 0.0f && AlertVariation <= 0.3f;
}
