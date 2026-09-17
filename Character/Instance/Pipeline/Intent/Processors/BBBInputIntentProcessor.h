
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBInputRuntimeData;

class ABBB_EVAC_API FBBBInputIntentProcessor final
{
public:

    /**
     * 将移动相关输入整理为角色意图
     * @param InputData	输入后处理数据
     * @param Intent	当前帧意图中间状态
     */
    void Update(const FBBBInputRuntimeData &InputData, FBBBCharacterIntentState &Intent) const;
};
