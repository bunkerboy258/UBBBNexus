#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterItemState.generated.h"

class FBBBCharacterDefaultItemInitializer;
class FBBBCharacterEquipmentProcessor;
class FBBBCharacterItemStorage;
class FBBBEquipmentExecutor;
class FBBBEquipmentRestoreProcessor;
class UBBBEquipmentInstance;
class UBBBItemInstance;

/** 角色唯一背包及其快捷访问绑定 */
USTRUCT(BlueprintType)
struct FBBBCharacterBackpackState
{
    GENERATED_BODY()

    /** 唯一保存角色拥有的物品实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UBBBItemInstance>> Slots;

    /** 直接引用背包实例的快捷操作槽位 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UBBBItemInstance>> QuickAccessBindings;
};

/** 角色当前期望与实际主手装备 */
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

/** 角色物品领域的背包与装备状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemState
{
    GENERATED_BODY()

    /** 角色唯一背包 */
    UPROPERTY(BlueprintReadOnly)
    FBBBCharacterBackpackState Backpack;

    /** 角色当前装备状态 */
    UPROPERTY(BlueprintReadOnly)
    FBBBCharacterEquipmentState Equipment;
};
