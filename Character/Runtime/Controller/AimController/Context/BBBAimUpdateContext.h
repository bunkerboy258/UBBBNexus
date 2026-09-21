#pragma once

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"

/** 本次瞄准更新在提交前使用的栈上暂存数据 */
struct FBBBAimUpdateContext final
{
    /** 本次更新正在组装的完整瞄准状态 */
    FBBBAimRuntimeState State;
};
