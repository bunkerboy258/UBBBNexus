#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/RuntimeData/BBBSMGRuntimeData.h"

struct FBBBSMGParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBSMGParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBSMGParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBSMGRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBSMGRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBSMGPrimaryLocalControlPacket>)
        {
            if (!Slot.bActive)
            {
                Slot.Packet.PressCount = 0;
            }
            Slot.Packet.PressCount += Packet.PressCount;
        }
        if constexpr (!std::is_same_v<TPacket, FBBBSMGPrimaryLocalControlPacket>)
        {
            if constexpr (std::is_same_v<TPacket, FBBBSMGEndContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGEndActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGBeginContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGBeginActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGFireRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGReloadStartRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGReloadEndRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGFireAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGReloadStartAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGReloadEndAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGVisibilityRemoteMessagePacket>)
            {
                Slot.Packet.Hidden.Append(Packet.Hidden);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBSMGVisibilityAuthorityFactPacket>)
            {
                Slot.Packet.Hidden.Append(Packet.Hidden);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else
            {
                Slot.Packet = MoveTemp(Packet);
            }
        }
        Slot.bActive = true;
    }

private:
    static TBBBSMGInputSlot<FBBBSMGEndContactLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBSMGInputSlot<FBBBSMGEndActionLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBSMGInputSlot<FBBBSMGBeginContactLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBSMGInputSlot<FBBBSMGBeginActionLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
    static TBBBSMGInputSlot<FBBBSMGEquipLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBSMGInputSlot<FBBBSMGEquipAuthorityFactPacket> &Select(FBBBSMGInputState &State, const FBBBSMGEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBSMGInputSlot<FBBBSMGPrimaryLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBSMGInputSlot<FBBBSMGSecondaryLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGSecondaryLocalControlPacket &)
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

    static TBBBSMGInputSlot<FBBBSMGLoadAmmoLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBSMGInputSlot<FBBBSMGInterruptReloadLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
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

    static TBBBSMGInputSlot<FBBBSMGHideLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBSMGInputSlot<FBBBSMGShowLocalControlPacket> &Select(FBBBSMGInputState &State, const FBBBSMGShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBSMGInputSlot<FBBBSMGVisibilityRemoteMessagePacket> &Select(FBBBSMGInputState &State, const FBBBSMGVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBSMGInputSlot<FBBBSMGVisibilityAuthorityFactPacket> &Select(FBBBSMGInputState &State, const FBBBSMGVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
