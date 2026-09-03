#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterLocomotionRuntimeData.generated.h"

/** 角色当前选择的移动步态 */
UENUM(BlueprintType)
enum class EBBBCharacterGait : uint8
{
    /** 行走 */
    Walk,

    /** 奔跑 */
    Run,

    /** 冲刺 */
    Sprint,

    /** 蹲伏移动 */
    Crouch
};

/** 角色移动系统持久运行事实 */
USTRUCT()
struct FBBBCharacterLocomotionRuntimeData
{
    GENERATED_BODY()

    /** @return 当前步态 */
    EBBBCharacterGait GetGait() const
    {
        return Gait;
    }

    /**
     * 提交当前步态
     * @param InGait 当前步态
     */
    void CommitGait(const EBBBCharacterGait InGait)
    {
        Gait = InGait;
    }

private:
    /** 当前本地计算或网络恢复后的步态 */
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;
};
