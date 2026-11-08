#pragma once
#include "Engine/DataAsset.h"
#include "BBBMonsterSpawnPoolEntry.h"
#include "BBBMonsterSpawnPoolDefinition.generated.h"

/** 可在多个副本阶段中复用的小怪组合 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterSpawnPoolDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 参与带权重选择的小怪模板 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪组合", meta = (DisplayName = "小怪列表"))
    TArray<FBBBMonsterSpawnPoolEntry> Entries;

    /**
     * @param Error	无效配置的中文原因
     * @return 是否存在完整有效的带权重组合
     */
    bool Validate(FString& Error) const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
