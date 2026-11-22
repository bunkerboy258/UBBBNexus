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
    /** 当前堆叠的数量 */
    int32 Quantity = 0;
    /** 所属快捷栏内的显示索引 */
    int32 QuickIndex = INDEX_NONE;
    /** 双击时的移动目标 可表示首个空闲武器快捷格 */
    int32 EquipSlot = INDEX_NONE;
    bool bOccupied = false;
    bool bQuick = false;
    bool bSelected = false;
    bool bActive = false;
    /** 需要显示堆叠数量 包括剩余一件 */
    bool bShowQuantity = false;
};
