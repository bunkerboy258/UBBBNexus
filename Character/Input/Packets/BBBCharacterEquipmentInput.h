#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterEquipmentInput.generated.h"

/** 角色支持的装备操作 */
UENUM(BlueprintType)
enum class EBBBCharacterActionType : uint8
{
    /** 无操作 */
    None,
    /** 切换装备 */
    Equip,
    /** 开火 */
    Fire,
    /** 换弹 */
    Reload
};

/** 外部提交的离散装备操作 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBBBCharacterActionType ActionType = EBBBCharacterActionType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EquipSlot = INDEX_NONE;
};

/** 装备已确认的操作阶段 */
UENUM(BlueprintType)
enum class EBBBCharacterEquipmentPhase : uint8
{
    /** 普通操作已执行 */
    Performed,
    /** 换弹已接受 */
    ReloadStarted,
    /** 弹夹已卸下 */
    MagazineDetached,
    /** 弹夹已装填 */
    MagazineLoaded,
    /** 换弹已取消 */
    ReloadCancelled
};

/** 装备提交给角色的已执行事实，同时用于角色侧还原 */
USTRUCT(BlueprintType)
struct FBBBEquipmentActionEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterActionType ActionType = EBBBCharacterActionType::None;

    UPROPERTY(BlueprintReadOnly)
    FName EquipmentId = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    int32 Sequence = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterEquipmentPhase Phase = EBBBCharacterEquipmentPhase::Performed;

    UPROPERTY(BlueprintReadOnly)
    int32 LoadedAmmo = 0;
};
