#pragma once

#include "CoreMinimal.h"

class UTexture2D;
class UMaterialInterface;

/** 控制器按需生成的只读展示数据 不保存物品容器或装备逻辑 */
struct FBBBPlayerItemDisplayData
{
    FText Name;
    FText Description;
    UTexture2D *Icon = nullptr;
    UTexture2D *DisplayImage = nullptr;
    UMaterialInterface *DisplayMaterial = nullptr;
    UTexture2D *PropertyIcon = nullptr;
    FGuid InstanceId;
    /** 双击时对应的快捷一号位或穿戴位置 无效值表示不可装备 */
    int32 EquipSlot = INDEX_NONE;
    bool bOccupied = false;
    bool bQuick = false;
    bool bSelected = false;
    bool bActive = false;
};
