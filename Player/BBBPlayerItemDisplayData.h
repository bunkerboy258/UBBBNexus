#pragma once

#include "CoreMinimal.h"

class UTexture2D;

/** 控制器按需生成的只读展示数据 不保存物品容器或装备逻辑 */
struct FBBBPlayerItemDisplayData
{
    FText Name;
    FText Description;
    UTexture2D *Icon = nullptr;
    bool bOccupied = false;
    bool bQuick = false;
    bool bSelected = false;
    bool bActive = false;
};
