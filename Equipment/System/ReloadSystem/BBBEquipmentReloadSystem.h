#pragma once
#include "CoreMinimal.h"
class ABBBEquipmentInstance;

/** 执行已批准的换弹阶段 */
class FBBBEquipmentReloadSystem final
{
public:
    /** @return 是否成功发布换弹表现 */
    bool Begin(ABBBEquipmentInstance &Instance, int32 Sequence) const;
    /** 卸下弹夹并清空当前弹量 */
    bool DetachMagazine(ABBBEquipmentInstance &Instance, int32 Sequence) const;
    /** 装填弹夹并结束换弹 */
    bool LoadMagazine(ABBBEquipmentInstance &Instance, int32 Sequence) const;
    /** 取消换弹并保留已执行的弹药结果 */
    bool Cancel(ABBBEquipmentInstance &Instance, int32 Sequence) const;
};
