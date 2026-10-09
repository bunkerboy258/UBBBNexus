#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/RuntimeData/BBBSMGRuntimeData.h"

struct FBBBSMGUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBSMGParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBSMGUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBSMGRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBSMGRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBSMGActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSMGFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSMGReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSMGReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSMGFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSMGReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSMGReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBSMGActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBSMGFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBSMGReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBSMGReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBSMGFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBSMGReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBSMGReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBSMGInputSlot<FBBBSMGEquipLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBSMGInputSlot<FBBBSMGEquipAuthorityFactPacket> &Select(FBBBSMGInputState &State, const FBBBSMGEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBSMGInputSlot<FBBBSMGFireLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGFireLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBSMGInputSlot<FBBBSMGReloadLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGReloadLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBSMGInputSlot<FBBBSMGBlockFireLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBSMGInputSlot<FBBBSMGAllowFireLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBSMGInputSlot<FBBBSMGLoadMagazineLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGLoadMagazineLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBSMGInputSlot<FBBBSMGInterruptReloadLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBSMGInputSlot<FBBBSMGActionPermissionLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBSMGInputSlot<FBBBSMGFireRemoteMessagePacket> &Select(FBBBSMGInputState &State, const FBBBSMGFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBSMGInputSlot<FBBBSMGReloadStartRemoteMessagePacket> &Select(FBBBSMGInputState &State, const FBBBSMGReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBSMGInputSlot<FBBBSMGReloadEndRemoteMessagePacket> &Select(FBBBSMGInputState &State, const FBBBSMGReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBSMGInputSlot<FBBBSMGFireAuthorityFactPacket> &Select(FBBBSMGInputState &State, const FBBBSMGFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBSMGInputSlot<FBBBSMGReloadStartAuthorityFactPacket> &Select(FBBBSMGInputState &State, const FBBBSMGReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBSMGInputSlot<FBBBSMGReloadEndAuthorityFactPacket> &Select(FBBBSMGInputState &State, const FBBBSMGReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
