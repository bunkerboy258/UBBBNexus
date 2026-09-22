#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"

struct FBBBCharacterControlState;

/** 本次瞄准更新在提交前使用的栈上暂存数据 */
struct FBBBAimUpdateContext final
{
    /** 本次更新维护的瞄准状态 */
    FBBBAimState &AimState;

    /** 输入系统已经裁决的角色控制状态 */
    const FBBBCharacterControlState &ControlState;
};
