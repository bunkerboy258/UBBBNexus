#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWeaponDefinition.generated.h"

class UBBBFireFragment;
class UBBBMagazineFragment;
class UBBBItemFragment;

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

    //收集武器层 Fragment 命名插槽
    virtual void CollectFragments(TArray<const UBBBItemFragment *> &OutFragments) const override;
};
