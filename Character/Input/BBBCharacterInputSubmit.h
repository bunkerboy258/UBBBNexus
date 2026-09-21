#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"

/**
 * 角色固定输入帧的唯一提交闸口
 *
 * 同类型输入只覆盖对应槽位，不分配节点、不增长容器；解析期间禁止重入写入，
 * 防止包方法仍在读取自身数据时被同类型提交覆盖
 */
namespace BBBCharacterInput
{
    /**
     * 提交输入包到对应固定槽位
     * @param Data	角色运行黑板
     * @param Packet	输入包
     * @return 是否接受输入
     */
    template<typename TPacket>
    bool Submit(FBBBCharacterRuntimeData &Data, TPacket &&Packet)
    {
        // 所有角色输入必须在游戏线程提交
        if (!IsInGameThread())
        {
            return false;
        }

        if (!Packet.IsValid())
        {
            return false;
        }

        if (Data.InputState.IsProcessing())
        {
            return false;
        }

        Data.InputState.Submit(Forward<TPacket>(Packet));
        return true;
    }
}
