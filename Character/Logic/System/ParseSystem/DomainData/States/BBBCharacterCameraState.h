#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBCharacterCameraState.generated.h"

/** 角色等待相机系统消费的表现输入状态 */
USTRUCT()
struct FBBBCharacterCameraState final
{
    GENERATED_BODY()

    /** 尚未被相机系统消费的最新完整输入 */
    FBBBPlayerCameraInput LatestInput;

    /** 最新贡献的发布序号 读取方独立记录消费进度 */
    uint64 Revision = 0;
};
