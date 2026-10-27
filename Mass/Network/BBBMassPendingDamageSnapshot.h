#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterDamageRemoteMessagePacket.h"

/** 出生前已收到的唯一当前贡献快照 不保存消息历史 */
struct FBBBMassPendingDamageSnapshot final
{
    /** 当前合并结果 */
    FBBBMonsterDamageRemoteMessagePacket Packet;

    /** 最近一次收到该当前结果的世界时间 */
    float UpdatedAt = 0.0f;
};
