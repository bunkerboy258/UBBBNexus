
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBInventoryTypes.h"
#include "BBBWork/UBBBNexus/Item/BBBItemTypes.h"
#include "BBBCharacterItemState.generated.h"
class ABBBEquipmentActor;
class UBBBEquipmentDefinition;
class FBBBCharacterDefaultItemInitializer;
class FBBBCharacterEquipmentSpawnProcessor;
class FBBBCharacterEquipmentTransitionProcessor;
class FBBBCharacterItemSystem;
class FBBBCharacterWeaponSwitchProcessor;
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

    const FBBBItemInstance &GetDesiredMainHandItem() const
    {
        return DesiredMainHandItem;
    }

    const FBBBItemInstance &GetActiveMainHandItem() const
    {
        return ActiveMainHandItem;
    }

    ABBBEquipmentActor *GetEquippedItemActor() const
    {
        return EquippedItemActor;
    }

    bool IsEquipping() const
    {
        return bIsEquipping;
    }

    /** @return 当前装备实体使用的静态定义 */
    const UBBBEquipmentDefinition *GetActiveMainHandDefinition() const
    {
        return ActiveMainHandDefinition;
    }
private:
    friend class FBBBCharacterDefaultItemInitializer;
    friend class FBBBCharacterEquipmentSpawnProcessor;
    friend class FBBBCharacterEquipmentTransitionProcessor;
    friend class FBBBCharacterItemSystem;
    friend class FBBBCharacterWeaponSwitchProcessor;
    friend class FBBBEquipmentExecutor;
    friend class FBBBEquipmentRestoreProcessor;

    void RestoreDesiredMainHandMirror(FName EquipmentHandle)
    {
        TargetMode = EBBBEquipmentTargetMode::Mirror;
        DesiredMainHandItem = FBBBItemInstance();
        DesiredMirrorHandle = EquipmentHandle;
    }

    EBBBEquipmentTargetMode TargetMode = EBBBEquipmentTargetMode::None;

    FName DesiredMirrorHandle = NAME_None;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FBBBItemInstance DesiredMainHandItem;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FBBBItemInstance ActiveMainHandItem;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> ActiveMainHandDefinition = nullptr;

    EBBBEquipmentTargetMode ActiveTargetMode = EBBBEquipmentTargetMode::None;

    FName ActiveMirrorHandle = NAME_None;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABBBEquipmentActor> EquippedItemActor = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool bIsEquipping = false;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float EquipEndTime = 0.0f;
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
