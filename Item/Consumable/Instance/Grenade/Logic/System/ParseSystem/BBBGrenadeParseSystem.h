#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include <type_traits>
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"
class ABBBGrenadeConsumable;
struct FBBBGrenadeRuntimeData;
class FBBBGrenadeInitializer;

/** 手雷Parse系统调度根 */
class FBBBGrenadeParseSystem final
{
public:
    /** @return 无 */
    void Update();
    /** @param Packet	待合并的固定输入 @return 是否接受 */
    template<typename TPacket>
    bool Submit(TPacket Packet);

private:
    friend class FBBBGrenadeInitializer;
    /** @param InActor	所属实例 @param InData	唯一黑板 @return 无 */
    void Initialize(ABBBGrenadeConsumable &InActor, FBBBGrenadeRuntimeData &InData);
    ABBBGrenadeConsumable *Actor = nullptr;
    FBBBGrenadeRuntimeData *Data = nullptr;
    FBBBGrenadeParseProcessor ParseProcessor;
};

template<typename TPacket>
bool FBBBGrenadeParseSystem::Submit(TPacket Packet)
{
    if (!Data)
    {
        return false;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumablePrimaryLocalControlPacket>)
    {
        auto &Slot = Data->Parse.InputState.PrimaryLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
        return true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumableEquipLocalControlPacket>)
    {
        auto &Slot = Data->Parse.InputState.EquipLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
        return true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumableReleaseLocalControlPacket>)
    {
        auto &Slot = Data->Parse.InputState.ReleaseLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
        return true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumableFinishLocalControlPacket>)
    {
        auto &Slot = Data->Parse.InputState.FinishLocalControl;
        Slot.Data = MoveTemp(Packet);
        Slot.bActive = true;
        return true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumablePermissionLocalControlPacket>)
    {
        auto &Slot = Data->Parse.InputState.PermissionLocalControl;
        Slot.Data.Values.Append(Packet.Values);
        Slot.bActive = true;
        return true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumableStateRemoteMessagePacket>)
    {
        auto &Slot = Data->Parse.InputState.StateRemoteMessage;
        Slot.Data.Facts.Append(Packet.Facts);
        Slot.bActive = true;
        return true;
    }
    if constexpr (std::is_same_v<TPacket, FBBBConsumableStateAuthorityFactPacket>)
    {
        auto &Slot = Data->Parse.InputState.StateAuthorityFact;
        Slot.Data.Facts.Append(Packet.Facts);
        Slot.bActive = true;
        return true;
    }
}
