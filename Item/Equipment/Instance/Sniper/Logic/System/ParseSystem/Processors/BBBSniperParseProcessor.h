#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"

struct FBBBSniperParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBSniperParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBSniperParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBSniperRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBSniperRuntimeData &Data, TPacket Packet)
    {
        if (!Packet.IsValid())
        {
            return false;
        }
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBSniperActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSniperFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSniperReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSniperReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSniperFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSniperReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBSniperReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBSniperActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBSniperFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBSniperReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBSniperReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBSniperFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBSniperReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBSniperReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBSniperInputSlot<FBBBSniperEquipLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBSniperInputSlot<FBBBSniperEquipAuthorityFactPacket> &Select(FBBBSniperInputState &State, const FBBBSniperEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBSniperInputSlot<FBBBSniperPrimaryLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBSniperInputSlot<FBBBSniperSecondaryLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperSecondaryLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBSniperInputSlot<FBBBSniperBlockFireLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBSniperInputSlot<FBBBSniperAllowFireLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBSniperInputSlot<FBBBSniperLoadAmmoLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBSniperInputSlot<FBBBSniperInterruptReloadLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBSniperInputSlot<FBBBSniperActionPermissionLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBSniperInputSlot<FBBBSniperFireRemoteMessagePacket> &Select(FBBBSniperInputState &State, const FBBBSniperFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBSniperInputSlot<FBBBSniperReloadStartRemoteMessagePacket> &Select(FBBBSniperInputState &State, const FBBBSniperReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBSniperInputSlot<FBBBSniperReloadEndRemoteMessagePacket> &Select(FBBBSniperInputState &State, const FBBBSniperReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBSniperInputSlot<FBBBSniperFireAuthorityFactPacket> &Select(FBBBSniperInputState &State, const FBBBSniperFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBSniperInputSlot<FBBBSniperReloadStartAuthorityFactPacket> &Select(FBBBSniperInputState &State, const FBBBSniperReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBSniperInputSlot<FBBBSniperReloadEndAuthorityFactPacket> &Select(FBBBSniperInputState &State, const FBBBSniperReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
