#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterOperationState.generated.h"

class ABBBEquipment;

/** 输入解析过程需要跨帧保留的操作状态 */
USTRUCT()
struct FBBBCharacterOperationState final
{
    GENERATED_BODY()

    /** 当前换弹所属装备 */
    UPROPERTY()
    TWeakObjectPtr<ABBBEquipment> ReloadEquipment;

    /** 本帧选择的目标装备 */
    UPROPERTY()
    TObjectPtr<ABBBEquipment> SelectedEquipment = nullptr;

    /** 当前换弹操作序号 */
    int32 ReloadSequence = INDEX_NONE;

    /** 最近完成的换弹操作序号 */
    int32 LastCompletedReloadSequence = INDEX_NONE;

    /** 本帧取消的换弹操作序号 */
    int32 CancelReloadSequence = INDEX_NONE;

    /** 当前换弹是否已经卸下弹匣 */
    bool bMagazineDetached = false;

    /** 当前换弹是否已经排队结束 */
    bool bEndQueued = false;

    /** 本帧是否已经承诺开火 */
    bool bFire = false;

    /** 本帧是否已经承诺换弹 */
    bool bReload = false;
};
