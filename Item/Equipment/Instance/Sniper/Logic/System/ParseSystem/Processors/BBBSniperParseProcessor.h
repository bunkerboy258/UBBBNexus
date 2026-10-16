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

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBSniperRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBSniperEndContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperEndActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperBeginContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperBeginActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperVisibilityRemoteMessagePacket>)
        {
            Slot.Packet.Hidden.Append(Packet.Hidden);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBSniperVisibilityAuthorityFactPacket>)
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
    static TBBBSniperInputSlot<FBBBSniperEndContactLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBSniperInputSlot<FBBBSniperEndActionLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBSniperInputSlot<FBBBSniperBeginContactLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBSniperInputSlot<FBBBSniperBeginActionLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
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

    static TBBBSniperInputSlot<FBBBSniperHideLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBSniperInputSlot<FBBBSniperShowLocalControlPacket> &Select(FBBBSniperInputState &State, const FBBBSniperShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBSniperInputSlot<FBBBSniperVisibilityRemoteMessagePacket> &Select(FBBBSniperInputState &State, const FBBBSniperVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBSniperInputSlot<FBBBSniperVisibilityAuthorityFactPacket> &Select(FBBBSniperInputState &State, const FBBBSniperVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
