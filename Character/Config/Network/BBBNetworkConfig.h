#pragma once

#include "CoreMinimal.h"
#include "BBBNetworkConfig.generated.h"

/** 角色网络系统配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterNetworkConfig
{
    GENERATED_BODY()

    /** 连续瞄准状态的最小上传间隔 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "瞄准上传间隔"))
    float AimUploadInterval = 0.033f;
};
