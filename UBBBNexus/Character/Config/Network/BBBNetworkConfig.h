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

    /** 控制者当前加速度快照的上传间隔 也用于丢包后的当前状态恢复 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "加速度上传间隔", ClampMin = "0.016"))
    float AccelerationUploadInterval = 0.05f;
};
