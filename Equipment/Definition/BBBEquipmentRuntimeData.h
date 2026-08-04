#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipmentRuntimeData.generated.h"

class UBBBEquipRuntimeData;
class UBBBEquipmentDefinition;
class UBBBEquipmentInstance;
class UBBBFireRuntimeData;
class UBBBMagazineRuntimeData;

/** 装备各领域运行数据根 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentRuntimeData final : public UObject
{
    GENERATED_BODY()

private:
    friend class UBBBEquipmentInstance;

    /**
     * 根据装备领域创建运行数据
     * @param Definition	装备静态配置
     */
    void Initialize(const UBBBEquipmentDefinition &Definition);

    /** @return 装备领域运行数据 */
    UBBBEquipRuntimeData *GetEquip() const;

    /** @return 开火领域运行数据 */
    UBBBFireRuntimeData *GetFire() const;

    /** @return 弹匣领域运行数据 */
    UBBBMagazineRuntimeData *GetMagazine() const;

    /** 装备领域运行数据 */
    UPROPERTY(Instanced)
    TObjectPtr<UBBBEquipRuntimeData> Equip = nullptr;

    /** 开火领域运行数据 */
    UPROPERTY(Instanced)
    TObjectPtr<UBBBFireRuntimeData> Fire = nullptr;

    /** 弹匣领域运行数据 */
    UPROPERTY(Instanced)
    TObjectPtr<UBBBMagazineRuntimeData> Magazine = nullptr;
};
