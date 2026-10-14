#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/RuntimeData/BBBRevolverRuntimeData.h"

struct FBBBRevolverParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBRevolverParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBRevolverParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBRevolverRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBRevolverRuntimeData &Data, TPacket Packet)
    {
        if (!Packet.IsValid())
        {
            return false;
        }
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBRevolverActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRevolverFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRevolverFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBRevolverActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBRevolverFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBRevolverReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBRevolverReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBRevolverFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBRevolverReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBRevolverReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBRevolverInputSlot<FBBBRevolverEquipLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBRevolverInputSlot<FBBBRevolverEquipAuthorityFactPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBRevolverInputSlot<FBBBRevolverPrimaryLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBRevolverInputSlot<FBBBRevolverSecondaryLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverSecondaryLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBRevolverInputSlot<FBBBRevolverBlockFireLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBRevolverInputSlot<FBBBRevolverAllowFireLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBRevolverInputSlot<FBBBRevolverLoadAmmoLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBRevolverInputSlot<FBBBRevolverInterruptReloadLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBRevolverInputSlot<FBBBRevolverActionPermissionLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBRevolverInputSlot<FBBBRevolverFireRemoteMessagePacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBRevolverInputSlot<FBBBRevolverReloadStartRemoteMessagePacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBRevolverInputSlot<FBBBRevolverReloadEndRemoteMessagePacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBRevolverInputSlot<FBBBRevolverFireAuthorityFactPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBRevolverInputSlot<FBBBRevolverReloadStartAuthorityFactPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBRevolverInputSlot<FBBBRevolverReloadEndAuthorityFactPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
