#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"

/**
 * 角色输入的唯一提交闸口
 * 所有输入统一视为离散快照 每帧压入黑板快照区队列
 * 闸口负责游戏线程检查与包自检 仲裁由解析系统负责
 */
namespace BBBCharacterInput
{
    /**
     * 提交离散快照包到本帧输入队列
     * @param Data	角色运行黑板
     * @param Packet	输入包
     * @return 是否接受输入
     */
    template<typename TPacket>
    bool Submit(FBBBCharacterRuntimeData &Data, TPacket &&Packet)
    {
        // 仅游戏线程可提交 包必须通过自检
        if (!ensureMsgf(IsInGameThread(), TEXT("[UBBBC]Packet input unavailable")))
        {
            return false;
        }

        if (!ensureMsgf(Packet.IsValid(), TEXT("[UBBBC]Invalid input packet")))
        {
            return false;
        }

        Data.Snapshot.Add(FBBBCharacterPacket(TInPlaceType<std::decay_t<TPacket>>{}, Forward<TPacket>(Packet)));
        return true;
    }
}
