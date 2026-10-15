#pragma once
#include "CoreMinimal.h"

class FSlateWindowElementList;
struct FGeometry;

namespace BBBCombatAmmoRail
{
    /**
     * 绘制准星右下角的短弧弹药细轨道
     * @param Elements	界面绘制列表
     * @param Geometry	战斗界面几何
     * @param Layer	绘制基础层
     * @param Loaded	当前弹药
     * @param Capacity	弹药容量
     * @param bContinuous	是否使用连续填充
     * @param Color	剩余弹药颜色
     * @return 无
     */
    void Paint(FSlateWindowElementList &Elements, const FGeometry &Geometry, int32 Layer,
        int32 Loaded, int32 Capacity, bool bContinuous, const FLinearColor &Color);
}
