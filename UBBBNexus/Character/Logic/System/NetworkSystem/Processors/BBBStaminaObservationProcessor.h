#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterNetworkUpdateContext;

/** 将本地体力结果变化上传到权威端并分发给镜像 */
class ABBB_EVAC_API FBBBStaminaObservationProcessor final
{
  public:
    /**
     * 上传发生变化的体力结果
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
