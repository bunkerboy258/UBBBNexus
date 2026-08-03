
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBInventoryTypes.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemTypes.h"
#include "BBBCharacterItemState.generated.h"
class UBBBEquipmentInstance;
class FBBBCharacterDefaultItemInitializer;
class FBBBCharacterEquipmentProcessor;
class FBBBEquipmentExecutor;
class FBBBEquipmentRestoreProcessor;

USTRUCT(BlueprintType)
struct FBBBCharacterItemInventoryState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FBBBInventoryList MainInventory;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGuid> HotbarItemInstanceIds;

    UPROPERTY(BlueprintReadOnly)
    int32 MainInventoryCapacity = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 HotbarCapacity = 0;
};

USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentState
{
    GENERATED_BODY()

    UBBBEquipmentInstance *GetDesiredMainHandInstance() const
    {
        return DesiredMainHandInstance;
    }

    UBBBEquipmentInstance *GetActiveMainHandInstance() const
    {
        return ActiveMainHandInstance;
    }

    bool IsEquipping() const;
private:
    friend class FBBBCharacterDefaultItemInitializer;
    friend class FBBBCharacterEquipmentProcessor;
    friend class FBBBEquipmentExecutor;
    friend class FBBBEquipmentRestoreProcessor;

    /** 角色期望装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentInstance> DesiredMainHandInstance = nullptr;

    /** 角色当前装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentInstance> ActiveMainHandInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FBBBCharacterItemState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FBBBCharacterItemInventoryState Inventory;

    UPROPERTY(BlueprintReadOnly)
    FBBBCharacterEquipmentState Equipment;
};
