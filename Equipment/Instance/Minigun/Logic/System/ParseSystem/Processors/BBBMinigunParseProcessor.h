#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"

struct FBBBMinigunUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBMinigunParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBMinigunUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBMinigunRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBMinigunRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBMinigunActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMinigunFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMinigunFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBMinigunActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBMinigunFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBMinigunReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBMinigunReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBMinigunFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBMinigunReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBMinigunReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBMinigunInputSlot<FBBBMinigunEquipLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBMinigunInputSlot<FBBBMinigunEquipAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBMinigunInputSlot<FBBBMinigunFireLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunFireLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBMinigunInputSlot<FBBBMinigunReloadLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunReloadLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBMinigunInputSlot<FBBBMinigunBlockFireLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBMinigunInputSlot<FBBBMinigunAllowFireLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBMinigunInputSlot<FBBBMinigunLoadMagazineLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunLoadMagazineLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBMinigunInputSlot<FBBBMinigunInterruptReloadLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBMinigunInputSlot<FBBBMinigunActionPermissionLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBMinigunInputSlot<FBBBMinigunFireRemoteMessagePacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBMinigunInputSlot<FBBBMinigunReloadStartRemoteMessagePacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBMinigunInputSlot<FBBBMinigunReloadEndRemoteMessagePacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBMinigunInputSlot<FBBBMinigunFireAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBMinigunInputSlot<FBBBMinigunReloadStartAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBMinigunInputSlot<FBBBMinigunReloadEndAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
