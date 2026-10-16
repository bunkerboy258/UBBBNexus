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

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBRevolverRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBRevolverEndContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverEndActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverBeginContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverBeginActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverVisibilityRemoteMessagePacket>)
        {
            Slot.Packet.Hidden.Append(Packet.Hidden);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRevolverVisibilityAuthorityFactPacket>)
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
    static TBBBRevolverInputSlot<FBBBRevolverEndContactLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBRevolverInputSlot<FBBBRevolverEndActionLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBRevolverInputSlot<FBBBRevolverBeginContactLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBRevolverInputSlot<FBBBRevolverBeginActionLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
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

    static TBBBRevolverInputSlot<FBBBRevolverHideLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBRevolverInputSlot<FBBBRevolverShowLocalControlPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBRevolverInputSlot<FBBBRevolverVisibilityRemoteMessagePacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBRevolverInputSlot<FBBBRevolverVisibilityAuthorityFactPacket> &Select(FBBBRevolverInputState &State, const FBBBRevolverVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
