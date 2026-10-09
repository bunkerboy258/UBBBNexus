#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"

struct FBBBShotgunUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBShotgunParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBShotgunUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBShotgunRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBShotgunRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBShotgunActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBShotgunFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBShotgunFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBShotgunActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBShotgunFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBShotgunReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBShotgunReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBShotgunFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBShotgunReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBShotgunReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBShotgunInputSlot<FBBBShotgunEquipLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBShotgunInputSlot<FBBBShotgunEquipAuthorityFactPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBShotgunInputSlot<FBBBShotgunFireLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunFireLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBShotgunInputSlot<FBBBShotgunReloadLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunReloadLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBShotgunInputSlot<FBBBShotgunBlockFireLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBShotgunInputSlot<FBBBShotgunAllowFireLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBShotgunInputSlot<FBBBShotgunLoadAmmoLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunLoadAmmoLocalControlPacket &)
    {
        return State.LoadAmmo;
    }

    static TBBBShotgunInputSlot<FBBBShotgunInterruptReloadLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBShotgunInputSlot<FBBBShotgunActionPermissionLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBShotgunInputSlot<FBBBShotgunFireRemoteMessagePacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBShotgunInputSlot<FBBBShotgunReloadStartRemoteMessagePacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBShotgunInputSlot<FBBBShotgunReloadEndRemoteMessagePacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBShotgunInputSlot<FBBBShotgunFireAuthorityFactPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBShotgunInputSlot<FBBBShotgunReloadStartAuthorityFactPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBShotgunInputSlot<FBBBShotgunReloadEndAuthorityFactPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
