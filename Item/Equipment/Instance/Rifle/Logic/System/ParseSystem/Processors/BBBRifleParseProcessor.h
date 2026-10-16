#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"

struct FBBBRifleParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBRifleParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBRifleParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBRifleRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBRifleRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBRifleEndContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleEndActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleBeginContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleBeginActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleFireRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleReloadStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleReloadEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleFireAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleReloadStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleReloadEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
            Slot.Packet.Completed.Append(Packet.Completed);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleVisibilityRemoteMessagePacket>)
        {
            Slot.Packet.Hidden.Append(Packet.Hidden);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBRifleVisibilityAuthorityFactPacket>)
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
    static TBBBRifleInputSlot<FBBBRifleEndContactLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBRifleInputSlot<FBBBRifleEndActionLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBRifleInputSlot<FBBBRifleBeginContactLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBRifleInputSlot<FBBBRifleBeginActionLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
    static TBBBRifleInputSlot<FBBBRifleEquipLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBRifleInputSlot<FBBBRifleEquipAuthorityFactPacket> &Select(FBBBRifleInputState &State, const FBBBRifleEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBRifleInputSlot<FBBBRiflePrimaryLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRiflePrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBRifleInputSlot<FBBBRifleSecondaryLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleSecondaryLocalControlPacket &)
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

    static TBBBRifleInputSlot<FBBBRifleLoadAmmoLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBRifleInputSlot<FBBBRifleInterruptReloadLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
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

    static TBBBRifleInputSlot<FBBBRifleHideLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBRifleInputSlot<FBBBRifleShowLocalControlPacket> &Select(FBBBRifleInputState &State, const FBBBRifleShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBRifleInputSlot<FBBBRifleVisibilityRemoteMessagePacket> &Select(FBBBRifleInputState &State, const FBBBRifleVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBRifleInputSlot<FBBBRifleVisibilityAuthorityFactPacket> &Select(FBBBRifleInputState &State, const FBBBRifleVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
