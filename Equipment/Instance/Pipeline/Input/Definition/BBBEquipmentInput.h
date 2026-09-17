#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Instance/ExternalAPI/Packets/BBBCharacterEquipmentEvent.h"
#include "BBBEquipmentInput.generated.h"

/** 装备接收的操作与事实类别 */
UENUM()
enum class EBBBEquipmentInputType : uint8
{
    /** 装备表现 */
    Equip,
    /** 开火请求 */
    Fire,
    /** 换弹请求 */
    Reload,
    /** 卸下弹夹 */
    DetachMagazine,
    /** 装填弹夹 */
    LoadMagazine,
    /** 取消换弹 */
    CancelReload,
    /** 取消此前尚未执行的操作 */
    CancelPendingActions,
    /** 镜像已确认事实 */
    Snapshot
};

/** 外部入口提交的有序输入 */
USTRUCT()
struct FBBBEquipmentInput
{
    GENERATED_BODY()

    UPROPERTY()
    EBBBEquipmentInputType Type = EBBBEquipmentInputType::Equip;

    UPROPERTY()
    int32 Sequence = INDEX_NONE;

    UPROPERTY()
    FBBBEquipmentActionEvent Snapshot;
};
