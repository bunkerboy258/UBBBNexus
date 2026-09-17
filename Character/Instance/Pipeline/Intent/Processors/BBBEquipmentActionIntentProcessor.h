
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBInputRuntimeData;

class ABBB_EVAC_API FBBBEquipmentActionIntentProcessor final
{
public:

    /**
     * 把开火与换弹输入转换为装备操作意图
     * @param InputData	输入后处理数据
     * @param Intent	当前帧意图中间状态
     */
    void Update(const FBBBInputRuntimeData &InputData, FBBBCharacterIntentState &Intent) const;
};
