#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAppearanceLoadState.generated.h"

struct FStreamableHandle;

/** 当前外观版本的资源请求 只有加载处理器维护 */
USTRUCT()
struct FBBBCharacterAppearanceLoadState final
{
    GENERATED_BODY()
    /** 请求所属的既成外观版本 */
    uint64 RequestedRevision = 0;
    /** 已消费的显示重试时刻 防止完成后重复发起同一次重试 */
    float RequestedRetryTime = 0.0f;
    /** 持有异步请求及其资源 不在回调内改写角色状态 */
    TSharedPtr<FStreamableHandle> Handle;
};
