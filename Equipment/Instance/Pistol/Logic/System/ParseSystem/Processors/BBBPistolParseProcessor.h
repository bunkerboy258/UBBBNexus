#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"

struct FBBBPistolUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBPistolParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBPistolUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBPistolRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBPistolRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBPistolActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBPistolFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBPistolReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBPistolReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBPistolFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBPistolReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBPistolReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBPistolActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBPistolFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBPistolReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBPistolReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBPistolFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBPistolReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBPistolReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBPistolInputSlot<FBBBPistolEquipLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBPistolInputSlot<FBBBPistolEquipAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBPistolInputSlot<FBBBPistolFireLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolFireLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBPistolInputSlot<FBBBPistolBlockFireLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBPistolInputSlot<FBBBPistolAllowFireLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBPistolInputSlot<FBBBPistolLoadMagazineLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolLoadMagazineLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBPistolInputSlot<FBBBPistolInterruptReloadLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBPistolInputSlot<FBBBPistolActionPermissionLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBPistolInputSlot<FBBBPistolFireRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadStartRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadEndRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBPistolInputSlot<FBBBPistolFireAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadStartAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadEndAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
