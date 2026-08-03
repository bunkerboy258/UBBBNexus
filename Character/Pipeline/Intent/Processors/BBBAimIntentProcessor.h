
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBInputRuntimeData;

class ABBB_EVAC_API FBBBAimIntentProcessor final
{
public:

    /**
     * 根据瞄准开火与宽限状态合成瞄准意图
     * @param InputData	输入后处理数据
     * @param Intent	当前帧意图中间状态
     */
    void Update(const FBBBInputRuntimeData &InputData, FBBBCharacterIntentState &Intent) const;
};
