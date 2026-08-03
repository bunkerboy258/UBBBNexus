
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBInputRuntimeData;

class ABBB_EVAC_API FBBBInputIntentProcessor final
{
public:

    /**
     * 直通拷贝基础移动与视角输入到意图状态
     * @param InputData	输入后处理数据
     * @param Intent	当前帧意图中间状态
     */
    void Update(const FBBBInputRuntimeData &InputData, FBBBCharacterIntentState &Intent) const;
};
