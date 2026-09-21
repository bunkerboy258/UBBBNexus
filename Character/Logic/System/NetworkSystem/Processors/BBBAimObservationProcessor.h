#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterNetworkUpdateContext;

/** 按节流条件观察并上传瞄准状态 */
class ABBB_EVAC_API FBBBAimObservationProcessor final
{
public:
    /**
     * 按节流条件上传瞄准状态
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
