#pragma once
struct FBBBGrenadeRuntimeData;
#include <type_traits>
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
struct FBBBGrenadeParseUpdateContext;
/** 手雷Parse处理器 */
class FBBBGrenadeParseProcessor final
{
public:
    /** @param Data 唯一黑板 @return 无 仅清除本领域状态 */
    static void Clear(FBBBGrenadeRuntimeData &Data);
    /** @param Data 唯一黑板 @param Packet 本类输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBGrenadeRuntimeData &Data, TPacket Packet);
    /** @param Context	本次领域上下文 @return 无 */
    void Update(FBBBGrenadeParseUpdateContext &Context) const;
};

template<typename TPacket>
void FBBBGrenadeParseProcessor::Submit(FBBBGrenadeRuntimeData &Data, TPacket Packet)
{
    static_assert(std::is_same_v<TPacket, FBBBGrenadeHideLocalControlPacket>
        || std::is_same_v<TPacket, FBBBGrenadeShowLocalControlPacket>
        || std::is_same_v<TPacket, FBBBGrenadePrimaryLocalControlPacket>
        || std::is_same_v<TPacket, FBBBGrenadeEquipLocalControlPacket>
        || std::is_same_v<TPacket, FBBBGrenadeReleaseLocalControlPacket>
        || std::is_same_v<TPacket, FBBBGrenadeFinishLocalControlPacket>
        || std::is_same_v<TPacket, FBBBGrenadeStateRemoteMessagePacket>
        || std::is_same_v<TPacket, FBBBGrenadeStateAuthorityFactPacket>);
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeHideLocalControlPacket>)
    {
        auto &Slot = Data.Parse.InputState.HideLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeShowLocalControlPacket>)
    {
        auto &Slot = Data.Parse.InputState.ShowLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadePrimaryLocalControlPacket>)
    {
        auto &Slot = Data.Parse.InputState.PrimaryLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeEquipLocalControlPacket>)
    {
        auto &Slot = Data.Parse.InputState.EquipLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeReleaseLocalControlPacket>)
    {
        auto &Slot = Data.Parse.InputState.ReleaseLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeFinishLocalControlPacket>)
    {
        auto &Slot = Data.Parse.InputState.FinishLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeStateRemoteMessagePacket>)
    {
        auto &Slot = Data.Parse.InputState.StateRemoteMessage;
        Slot.Data.Facts.Append(Packet.Facts);
        Slot.bActive = true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBGrenadeStateAuthorityFactPacket>)
    {
        auto &Slot = Data.Parse.InputState.StateAuthorityFact;
        Slot.Data.Facts.Append(Packet.Facts);
        Slot.bActive = true;
    }
}
