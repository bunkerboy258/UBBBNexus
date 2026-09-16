#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBCharacterEquipmentStates.generated.h"

class FBBBCharacterDefaultEquipmentInitializer;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterEquipmentActionProcessor;
class FBBBEquipmentRestoreProcessor;
class FBBBEquipmentSelectionExecutor;

/** 角色拥有的装备及快捷访问绑定 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentInventoryState
{
    GENERATED_BODY()

    /** 角色拥有的全部装备实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<ABBBEquipmentInstance>> Slots;

    /** 直接引用装备实例的快捷操作槽位 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<ABBBEquipmentInstance>> QuickAccessBindings;
};

/** 角色期望与当前主手装备状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentState
{
    GENERATED_BODY()

    /** @return 角色期望装备的主手实例 */
    ABBBEquipmentInstance *GetDesiredMainHandInstance() const
    {
        return DesiredMainHandInstance;
    }

    /** @return 角色当前装备的主手实例 */
    ABBBEquipmentInstance *GetActiveMainHandInstance() const
    {
        return ActiveMainHandInstance;
    }

    /** @return 角色正在等待换弹动画阶段 */
    bool IsReloading() const
    {
        return ReloadSequence > 0;
    }

    /** @return 当前换弹身份 */
    int32 GetReloadSequence() const
    {
        return ReloadSequence;
    }

private:
    friend class FBBBCharacterDefaultEquipmentInitializer;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBEquipmentRestoreProcessor;
    friend class FBBBEquipmentSelectionExecutor;

    /** 角色期望装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABBBEquipmentInstance> DesiredMainHandInstance = nullptr;

    /** 角色当前装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABBBEquipmentInstance> ActiveMainHandInstance = nullptr;

    /** 当前装备持续动作 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 ReloadSequence = INDEX_NONE;

    /** 下一个本地动作顺序号 */
    int32 NextActionSequence = 1;
};
