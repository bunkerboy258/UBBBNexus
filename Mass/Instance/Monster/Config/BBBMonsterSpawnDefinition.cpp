#include "BBBMonsterSpawnDefinition.h"
#include "BBBMonsterSpawnPoolDefinition.h"

bool UBBBMonsterSpawnDefinition::Validate(FString& Error) const
{
    if (!Pool || !Pool->Validate(Error))
    {
        Error = TEXT("出生组合无效 ") + Error;
        return false;
    }
    if (CandidateCount < 1 || CandidateCount > 512 || CandidatesPerFrame < 1 || CandidatesPerFrame > 64
        || CreatesPerFrame < 1 || CreatesPerFrame > 64 || MaxObservers < 1 || MaxObservers > 16
        || WorldMaxAlive < 1 || WorldMaxAlive > 4096
        || !FMath::IsFinite(TimeBudgetMs) || TimeBudgetMs < 0.1f || TimeBudgetMs > 16.0f
        || !FMath::IsFinite(MinPlayerDistance) || MinPlayerDistance < 0.0f
        || !FMath::IsFinite(MinSpacing) || MinSpacing <= 0.0f
        || !FMath::IsFinite(GroundTraceDistance) || GroundTraceDistance <= 0.0f || !FMath::IsFinite(FixedYaw))
    {
        Error = TEXT("出生预算 距离或朝向参数无效");
        return false;
    }
    Error.Reset();
    return true;
}
