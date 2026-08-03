#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWeaponDefinition.generated.h"

/** 由武器实体和武器能力片段驱动的具体装备定义 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBWeaponDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()
};
