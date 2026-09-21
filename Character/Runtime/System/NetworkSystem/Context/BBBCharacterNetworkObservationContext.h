#pragma once

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkStates.h"

/** 本次瞄准网络观察与提交使用的栈上暂存数据 */
struct FBBBAimObservationContext final
{
    /** 上一次已经提交的瞄准观察结果 */
    FBBBAimNetworkObserverState Observer;

    /** 本次准备提交的瞄准状态 */
    FBBBAimNetworkState State;
};

/** 本次移动网络观察与提交使用的栈上暂存数据 */
struct FBBBLocomotionObservationContext final
{
    /** 当前角色步态 */
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;

    /** 本次准备提交的移动状态 */
    FBBBLocomotionNetworkState State;

    /** 本次提交后需要保存的观察结果 */
    FBBBLocomotionNetworkObserverState Observer;
};
