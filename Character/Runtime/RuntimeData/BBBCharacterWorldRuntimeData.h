#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterWorldRuntimeData.generated.h"
/** 角色当前帧读取的世界时间快照 */
USTRUCT()
struct FBBBCharacterWorldRuntimeData
{
    GENERATED_BODY()
    /** 当前角色帧间隔 */
    UPROPERTY(Transient)
    float FrameDeltaSeconds = 0.0f;

    /** 当前世界游戏时间 */
    UPROPERTY(Transient)
    float WorldTimeSeconds = 0.0f;
};
