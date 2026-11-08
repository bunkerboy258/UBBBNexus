#include "BBBMonsterSpawnPoolDefinition.h"
#include "BBBMonsterDefinition.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

bool UBBBMonsterSpawnPoolDefinition::Validate(FString& Error) const
{
    double Sum = 0.0;
    TSet<const UBBBMonsterDefinition*> Seen;
    for (const auto& Entry : Entries)
    {
        if (!FMath::IsFinite(Entry.Weight) || Entry.Weight < 0.0f || !Entry.Definition || !Entry.Definition->IsValid()
            || Seen.Contains(Entry.Definition))
        {
            Error = TEXT("小怪组合包含无效定义 重复定义或错误权重");
            return false;
        }
        Seen.Add(Entry.Definition);
        Sum += Entry.Weight;
    }
    if (Sum <= 0.0 || !FMath::IsFinite(Sum))
    {
        Error = TEXT("小怪组合缺少正权重模板");
        return false;
    }
    Error.Reset();
    return true;
}

#if WITH_EDITOR
EDataValidationResult UBBBMonsterSpawnPoolDefinition::IsDataValid(FDataValidationContext& Context) const
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
