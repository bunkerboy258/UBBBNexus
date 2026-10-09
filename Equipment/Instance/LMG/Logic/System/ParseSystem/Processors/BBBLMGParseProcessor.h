#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"

struct FBBBLMGUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBLMGParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBLMGUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBLMGRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBLMGRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBLMGActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBLMGFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBLMGReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBLMGReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBLMGFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBLMGReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBLMGReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBLMGActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBLMGFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBLMGReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBLMGReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBLMGFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBLMGReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBLMGReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBLMGInputSlot<FBBBLMGEquipLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBLMGInputSlot<FBBBLMGEquipAuthorityFactPacket> &Select(FBBBLMGInputState &State, const FBBBLMGEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBLMGInputSlot<FBBBLMGFireLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGFireLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBLMGInputSlot<FBBBLMGReloadLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGReloadLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBLMGInputSlot<FBBBLMGBlockFireLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBLMGInputSlot<FBBBLMGAllowFireLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBLMGInputSlot<FBBBLMGLoadMagazineLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGLoadMagazineLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBLMGInputSlot<FBBBLMGInterruptReloadLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBLMGInputSlot<FBBBLMGActionPermissionLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBLMGInputSlot<FBBBLMGFireRemoteMessagePacket> &Select(FBBBLMGInputState &State, const FBBBLMGFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBLMGInputSlot<FBBBLMGReloadStartRemoteMessagePacket> &Select(FBBBLMGInputState &State, const FBBBLMGReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBLMGInputSlot<FBBBLMGReloadEndRemoteMessagePacket> &Select(FBBBLMGInputState &State, const FBBBLMGReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBLMGInputSlot<FBBBLMGFireAuthorityFactPacket> &Select(FBBBLMGInputState &State, const FBBBLMGFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBLMGInputSlot<FBBBLMGReloadStartAuthorityFactPacket> &Select(FBBBLMGInputState &State, const FBBBLMGReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBLMGInputSlot<FBBBLMGReloadEndAuthorityFactPacket> &Select(FBBBLMGInputState &State, const FBBBLMGReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
