#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterNetworkUpdateContext;

/** 只观察控制者已经产生的加速度并同步当前结果 */
class ABBB_EVAC_API FBBBAccelerationObservationProcessor final
{
public:
    /** @param Context 本次网络观察上下文 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
