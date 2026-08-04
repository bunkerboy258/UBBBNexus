#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentDefinition.h"
#include "BBBWeaponDefinition.generated.h"

class UBBBFireFragment;
class UBBBMagazineFragment;

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBWeaponDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()
public:

    //开火配置插槽
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Weapon")
    TObjectPtr<UBBBFireFragment> FireFragment;

    //弹匣配置插槽
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Weapon")
    TObjectPtr<UBBBMagazineFragment> MagazineFragment;
};
