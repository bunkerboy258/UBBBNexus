#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBCharacterItemStates.generated.h"

class FBBBCharacterDefaultItemInitializer;
class FBBBCharacterEquipmentProcessor;
class FBBBCharacterItemStorage;
class FBBBEquipmentExecutor;
class FBBBEquipmentRestoreProcessor;
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
    UBBBItemInstance *GetDesiredMainHandInstance() const
    {
        return DesiredMainHandInstance;
    }

    /** @return 角色当前装备的主手实例 */
    UBBBItemInstance *GetActiveMainHandInstance() const
    {
        return ActiveMainHandInstance;
    }

    /** @return 当前装备是否仍在执行装备过渡 */
    bool IsEquipping() const
    {
        return ActiveMainHandInstance && ActiveMainHandInstance->IsEquipping();
    }

private:

    friend class FBBBCharacterDefaultItemInitializer;
    friend class FBBBCharacterEquipmentProcessor;
    friend class FBBBEquipmentExecutor;
    friend class FBBBEquipmentRestoreProcessor;

    /** 角色期望装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemInstance> DesiredMainHandInstance = nullptr;

    /** 角色当前装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemInstance> ActiveMainHandInstance = nullptr;
};
