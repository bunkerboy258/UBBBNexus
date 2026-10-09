#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"

struct FBBBRifleUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBRifleParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBRifleRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBRifleRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBRifleActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRifleFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRifleReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRifleReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRifleFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRifleReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBRifleReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBRifleActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBRifleFireRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBRifleReloadStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBRifleReloadEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBRifleFireAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBRifleReloadStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBRifleReloadEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBRifleInputSlot<FBBBRifleEquipLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBRifleInputSlot<FBBBRifleEquipAuthorityFactPacket> &Select(FBBBRifleInputState &State, const FBBBRifleEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBRifleInputSlot<FBBBRifleFireLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleFireLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBRifleInputSlot<FBBBRifleReloadLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleReloadLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBRifleInputSlot<FBBBRifleBlockFireLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBRifleInputSlot<FBBBRifleAllowFireLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBRifleInputSlot<FBBBRifleLoadMagazineLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleLoadMagazineLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBRifleInputSlot<FBBBRifleInterruptReloadLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }

    static TBBBRifleInputSlot<FBBBRifleActionPermissionLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
    }

    static TBBBRifleInputSlot<FBBBRifleFireRemoteMessagePacket> &Select(FBBBRifleInputState &State, const FBBBRifleFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBRifleInputSlot<FBBBRifleReloadStartRemoteMessagePacket> &Select(FBBBRifleInputState &State, const FBBBRifleReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBRifleInputSlot<FBBBRifleReloadEndRemoteMessagePacket> &Select(FBBBRifleInputState &State, const FBBBRifleReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBRifleInputSlot<FBBBRifleFireAuthorityFactPacket> &Select(FBBBRifleInputState &State, const FBBBRifleFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBRifleInputSlot<FBBBRifleReloadStartAuthorityFactPacket> &Select(FBBBRifleInputState &State, const FBBBRifleReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBRifleInputSlot<FBBBRifleReloadEndAuthorityFactPacket> &Select(FBBBRifleInputState &State, const FBBBRifleReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

};
