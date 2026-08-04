
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBInputRuntimeData;

class ABBB_EVAC_API FBBBEquipmentSelectionIntentProcessor final
{
public:

    /**
     * 按槽位按键优先级生成装备切换意图
     * @param InputData	输入后处理数据
     * @param Intent	当前帧意图中间状态
     */
    void Update(const FBBBInputRuntimeData &InputData, FBBBCharacterIntentState &Intent) const;
};
