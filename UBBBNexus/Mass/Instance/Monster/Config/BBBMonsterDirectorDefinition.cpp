#include "BBBMonsterDirectorDefinition.h"
#include "BBBMonsterSpawnDefinition.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

const FBBBMonsterDirectorEventWaveDefinition* UBBBMonsterDirectorDefinition::FindEventWave(const FName EventId) const
{
    if (EventId.IsNone())
    {
        return nullptr;
    }
    return EventWaves.FindByPredicate([EventId](const FBBBMonsterDirectorEventWaveDefinition& Wave)
    {
        return Wave.EventId == EventId;
    });
}

bool UBBBMonsterDirectorDefinition::Validate(FString& Error) const
{
    if (WorldMaxAlive < 1 || !FMath::IsFinite(AdditionalPlayerScale) || AdditionalPlayerScale < 0.0f || AdditionalPlayerScale > 1.0f
        || !FMath::IsFinite(DespawnDistance) || DespawnDistance < 0.0f
        || !FMath::IsFinite(RetryInterval) || RetryInterval < 0.1f
        || !FMath::IsFinite(QueryTimeout) || QueryTimeout < 1.0f
        || !FMath::IsFinite(UpdateInterval) || UpdateInterval < 0.05f
        || !FMath::IsFinite(PopulationActivationRadius) || PopulationActivationRadius < 500.0f)
    {
        Error = TEXT("导演全局参数或固有激活半径无效");
        return false;
    }
    if (PopulationSpawnDefinition && !PopulationSpawnDefinition->Validate(Error))
    {
        Error = TEXT("默认固有出生配置无效 ") + Error;
        return false;
    }
    if (bEnableDynamicHorde)
    {
        if (!DynamicHordeSpawnDefinition || !DynamicHordeSpawnDefinition->Validate(Error))
        {
            Error = TEXT("动态波次出生配置无效 ") + Error;
            return false;
        }
        if (!FMath::IsFinite(DynamicHordeMinInterval) || !FMath::IsFinite(DynamicHordeMaxInterval)
            || DynamicHordeMinInterval < 5.0f || DynamicHordeMaxInterval < DynamicHordeMinInterval
            || DynamicHordeBatchCount < 1 || DynamicHordeBatchCount > 128
            || !FMath::IsFinite(DynamicHordeSampleRadius) || DynamicHordeSampleRadius < 500.0f
            || !FMath::IsFinite(DynamicHordeSampleHalfHeight) || DynamicHordeSampleHalfHeight < 100.0f)
        {
            Error = TEXT("动态波次时间 数量或采样范围参数无效");
            return false;
        }
    }
    TSet<FName> SeenEvents;
    for (const auto& Wave : EventWaves)
    {
        if (Wave.EventId.IsNone() || SeenEvents.Contains(Wave.EventId))
        {
            Error = TEXT("预制波次标识为空或重复");
            return false;
        }
        SeenEvents.Add(Wave.EventId);
        if (!Wave.SpawnDefinition || !Wave.SpawnDefinition->Validate(Error))
        {
            Error = FString::Printf(TEXT("预制波次 %s 出生配置无效 %s"), *Wave.EventId.ToString(), *Error);
            return false;
        }
        if (Wave.BatchCount < 1 || Wave.MaxAlive < 1 || !FMath::IsFinite(Wave.Interval) || Wave.Interval < 0.5f)
        {
            Error = FString::Printf(TEXT("预制波次 %s 数量或间隔无效"), *Wave.EventId.ToString());
            return false;
        }
        if (Wave.Mode == EBBBMonsterDirectorEventWaveMode::Burst && Wave.TotalCount < 1)
        {
            Error = FString::Printf(TEXT("单波爆发事件 %s 的总生成上限必须大于零"), *Wave.EventId.ToString());
            return false;
        }
        for (const FName RegionId : Wave.RegionIds)
        {
            if (RegionId.IsNone())
            {
                Error = FString::Printf(TEXT("预制波次 %s 包含空区域标识"), *Wave.EventId.ToString());
                return false;
            }
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
