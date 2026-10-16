#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"

struct FBBBMinigunParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBMinigunParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBMinigunParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBMinigunRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBMinigunRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBMinigunEndContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunEndActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunBeginContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunBeginActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunSpinRemoteMessagePacket>)
        {
            Slot.Packet.Spinning.Append(Packet.Spinning);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunSpinAuthorityFactPacket>)
        {
            Slot.Packet.Spinning.Append(Packet.Spinning);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunVisibilityRemoteMessagePacket>)
        {
            Slot.Packet.Hidden.Append(Packet.Hidden);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMinigunVisibilityAuthorityFactPacket>)
        {
            Slot.Packet.Hidden.Append(Packet.Hidden);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
    }

private:
    static TBBBMinigunInputSlot<FBBBMinigunEndContactLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBMinigunInputSlot<FBBBMinigunEndActionLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBMinigunInputSlot<FBBBMinigunBeginContactLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBMinigunInputSlot<FBBBMinigunBeginActionLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
    static TBBBMinigunInputSlot<FBBBMinigunSpinRemoteMessagePacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunSpinRemoteMessagePacket &)
    {
        return State.RemoteSpin;
    }

    static TBBBMinigunInputSlot<FBBBMinigunSpinAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunSpinAuthorityFactPacket &)
    {
        return State.AuthoritySpin;
    }

    static TBBBMinigunInputSlot<FBBBMinigunEquipLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBMinigunInputSlot<FBBBMinigunEquipAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBMinigunInputSlot<FBBBMinigunPrimaryLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBMinigunInputSlot<FBBBMinigunSecondaryLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunSecondaryLocalControlPacket &)
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

    static TBBBMinigunInputSlot<FBBBMinigunLoadAmmoLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBMinigunInputSlot<FBBBMinigunInterruptReloadLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
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

    static TBBBMinigunInputSlot<FBBBMinigunHideLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBMinigunInputSlot<FBBBMinigunShowLocalControlPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBMinigunInputSlot<FBBBMinigunVisibilityRemoteMessagePacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBMinigunInputSlot<FBBBMinigunVisibilityAuthorityFactPacket> &Select(FBBBMinigunInputState &State, const FBBBMinigunVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
