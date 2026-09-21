#pragma once

#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBCharacterMontageRequestState.h"

/** 本次蒙太奇请求校验或应用使用的栈上暂存数据 */
struct FBBBCharacterMontageRequestContext final
{
    /** 本次正在组装的蒙太奇请求 */
    FBBBCharacterMontageRequestState Request;
};

/** 本次蒙太奇输入提交使用的栈上暂存数据 */
struct FBBBCharacterMontageSubmitContext final
{
    /** 本次等待映射到具体固定槽位包的数据 */
    FBBBMontagePacketData Data;
};
