#pragma once
#include "UObject/Object.h"
#include "BBBMonsterDismembermentPartDefinition.h"
#include "BBBMonsterDismembermentDefinition.generated.h"

/** 头部与四肢的预制断点资源 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, HideDropdown, meta=(DisplayName="断肢系统"))
class ABBB_EVAC_API UBBBMonsterDismembermentDefinition final : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="断肢", meta=(DisplayName="预制断点", TitleProperty="Name"))
    TArray<FBBBMonsterDismembermentPartDefinition> Parts;

    /** @param Id 稳定断点编号 @return 配对资源或空 */
    const FBBBMonsterDismembermentPartDefinition* Find(uint8 Id) const
    {
        return Parts.FindByPredicate([Id](const auto& Part)
        {
            return Part.Id == Id;
        });
    }
};
