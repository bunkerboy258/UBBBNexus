#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBEquipmentCatalog.generated.h"

class UBBBEquipmentDefinition;

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    /**
     * 按装备句柄查找装备静态配置
     * @param EquipmentHandle	装备句柄 对应装备配置的 ItemId
     * @return 匹配的装备静态配置 未找到时返回 nullptr
     */
    UBBBEquipmentDefinition *FindDefinition(FName EquipmentHandle);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TArray<TObjectPtr<UBBBEquipmentDefinition>> Definitions;
};
