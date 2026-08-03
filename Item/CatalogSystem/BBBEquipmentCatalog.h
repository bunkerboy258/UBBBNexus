#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBEquipmentCatalog.generated.h"

class UBBBEquipmentDefinition;

/** 统一保存可由句柄重建的装备定义 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    /**
     * 按句柄查找装备定义
     * @param EquipmentHandle	装备句柄
     * @return 与句柄匹配的装备定义
     */
    UBBBEquipmentDefinition *FindDefinition(FName EquipmentHandle);

    /** 可由远端镜像重建的装备定义 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TArray<TObjectPtr<UBBBEquipmentDefinition>> Definitions;
};
