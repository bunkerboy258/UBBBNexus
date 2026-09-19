#pragma once

#include "CoreMinimal.h"

class UBBBEquipmentCatalog;
struct FBBBCharacterRuntimeData;

/** 将外部输入包按固定优先级写入角色黑板 */
class FBBBCharacterInputProcessor final
{
public:
    /**
     * 解析本帧输入包并发布已裁决控制
     * @param Data	角色黑板
     * @param Catalog	装备目录
     * @param bRestoreMode	是否还原模式
     */
    void Update(FBBBCharacterRuntimeData &Data, UBBBEquipmentCatalog &Catalog, bool bRestoreMode) const;

private:
    /** 派生门控与已裁决控制发布 */
    static void FinalizeControl(FBBBCharacterRuntimeData &Data);
};
