#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentActionFact.generated.h"

/** 已成立的装备动作类型 */
UENUM(BlueprintType)
enum class EBBBEquipmentActionType : uint8
{
    /** 无有效动作 */
    None,

    /** 装备完成 */
    Equip,

    /** 开火完成 */
    Fire,

    /** 换弹开始 */
    ReloadStarted,

    /** 弹匣脱离 */
    MagazineDetached,

    /** 弹匣装入 */
    MagazineLoaded,

    /** 换弹取消 */
    ReloadCancelled
};

/**
 * 装备已确认动作的事实记录
 * 角色事件留档、装备还原与网络传输共用同一份领域事实
 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentActionFact
{
    GENERATED_BODY()

    /** 已成立的装备动作类型 */
    UPROPERTY(BlueprintReadOnly)
    EBBBEquipmentActionType Type = EBBBEquipmentActionType::None;

    /** 事实所属装备标识 */
    UPROPERTY(BlueprintReadOnly)
    FName EquipmentId = NAME_None;

    /** 事实所属操作序号 */
    UPROPERTY(BlueprintReadOnly)
    int32 Sequence = INDEX_NONE;

    /** 事实成立后的装填弹药数量 */
    UPROPERTY(BlueprintReadOnly)
    int32 LoadedAmmo = 0;
};
