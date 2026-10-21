#include "BBBMonsterDirectorDefinition.h"
#include "BBBMonsterSpawnDefinition.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

bool UBBBMonsterDirectorDefinition::Validate(FString& Error) const
{
    if (Stages.IsEmpty() || WorldMaxAlive < 1
        || !FMath::IsFinite(RetryInterval) || RetryInterval < 0.1f
        || !FMath::IsFinite(QueryTimeout) || QueryTimeout < 1.0f
        || !FMath::IsFinite(UpdateInterval) || UpdateInterval < 0.05f)
    {
        Error = TEXT("导演需要阶段及有效的数量与时间参数");
        return false;
    }
    TSet<FName> StageIds;
    for (const auto& Stage : Stages)
    {
        if (Stage.Id.IsNone() || StageIds.Contains(Stage.Id) || !Stage.SpawnDefinition || !Stage.SpawnDefinition->Validate(Error)
            || static_cast<uint8>(Stage.Mode) > static_cast<uint8>(EBBBMonsterDirectorSpawnMode::Refill)
            || static_cast<uint8>(Stage.Activation) > static_cast<uint8>(EBBBMonsterDirectorActivation::External)
            || Stage.TotalCount < 1 || Stage.BatchCount < 1 || Stage.MaxAlive < 1
            || Stage.BatchCount > Stage.MaxAlive || Stage.MaxAlive > WorldMaxAlive
            || !FMath::IsFinite(Stage.Interval) || Stage.Interval < 0.1f || Stage.RegionIds.IsEmpty()
            || (Stage.Mode == EBBBMonsterDirectorSpawnMode::Once && Stage.TotalCount > Stage.MaxAlive))
        {
            Error = FString::Printf(TEXT("阶段 %s 的标识 组合 数量 时间或区域无效 %s"), *Stage.Id.ToString(), *Error);
            return false;
        }
        StageIds.Add(Stage.Id);
        TSet<FName> Regions;
        for (const FName Id : Stage.RegionIds)
        {
            if (Id.IsNone() || Regions.Contains(Id))
            {
                Error = FString::Printf(TEXT("阶段 %s 包含空区域或重复区域"), *Stage.Id.ToString());
                return false;
            }
            Regions.Add(Id);
        }
    }
    Error.Reset();
    return true;
}

#if WITH_EDITOR
EDataValidationResult UBBBMonsterDirectorDefinition::IsDataValid(FDataValidationContext& Context) const
{
    FString Error;
    if (!Validate(Error))
    {
        Context.AddError(FText::FromString(Error));
        return EDataValidationResult::Invalid;
    }
    return EDataValidationResult::Valid;
}
#endif
