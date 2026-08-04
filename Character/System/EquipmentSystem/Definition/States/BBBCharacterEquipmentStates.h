#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBCharacterEquipmentStates.generated.h"

class FBBBCharacterDefaultEquipmentInitializer;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBEquipmentRestoreProcessor;
class FBBBEquipmentSelectionExecutor;

/** 角色拥有的装备及快捷访问绑定 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentInventoryState
{
    GENERATED_BODY()

    /** 角色拥有的全部装备实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UBBBEquipmentInstance>> Slots;

    /** 直接引用装备实例的快捷操作槽位 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UBBBEquipmentInstance>> QuickAccessBindings;
};

/** 角色期望与当前主手装备状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentState
{
    GENERATED_BODY()

    /** @return 角色期望装备的主手实例 */
    UBBBEquipmentInstance *GetDesiredMainHandInstance() const
    {
        return DesiredMainHandInstance;
    }

    /** @return 角色当前装备的主手实例 */
    UBBBEquipmentInstance *GetActiveMainHandInstance() const
    {
        return ActiveMainHandInstance;
    }

    /** @return 当前装备是否仍在执行装备过渡 */
    bool IsEquipping() const
    {
        return ActiveMainHandInstance && ActiveMainHandInstance->IsEquipping();
    }

private:
    friend class FBBBCharacterDefaultEquipmentInitializer;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBEquipmentRestoreProcessor;
    friend class FBBBEquipmentSelectionExecutor;

    /** 角色期望装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentInstance> DesiredMainHandInstance = nullptr;

    /** 角色当前装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentInstance> ActiveMainHandInstance = nullptr;
};
