#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"

struct FBBBLMGParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBLMGParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBLMGParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBLMGRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBLMGRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBLMGPrimaryLocalControlPacket>)
        {
            if (!Slot.bActive)
            {
                Slot.Packet.PressCount = 0;
            }
            Slot.Packet.PressCount += Packet.PressCount;
        }
        if constexpr (!std::is_same_v<TPacket, FBBBLMGPrimaryLocalControlPacket>)
        {
            if constexpr (std::is_same_v<TPacket, FBBBLMGEndContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGEndActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGBeginContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGBeginActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGFireRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGReloadStartRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGReloadEndRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGFireAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGReloadStartAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGReloadEndAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGVisibilityRemoteMessagePacket>)
            {
                Slot.Packet.Hidden.Append(Packet.Hidden);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBLMGVisibilityAuthorityFactPacket>)
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
    static TBBBLMGInputSlot<FBBBLMGEndContactLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBLMGInputSlot<FBBBLMGEndActionLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBLMGInputSlot<FBBBLMGBeginContactLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBLMGInputSlot<FBBBLMGBeginActionLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
    static TBBBLMGInputSlot<FBBBLMGEquipLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBLMGInputSlot<FBBBLMGEquipAuthorityFactPacket> &Select(FBBBLMGInputState &State, const FBBBLMGEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBLMGInputSlot<FBBBLMGPrimaryLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBLMGInputSlot<FBBBLMGSecondaryLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGSecondaryLocalControlPacket &)
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

    static TBBBLMGInputSlot<FBBBLMGLoadAmmoLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBLMGInputSlot<FBBBLMGInterruptReloadLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
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

    static TBBBLMGInputSlot<FBBBLMGHideLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBLMGInputSlot<FBBBLMGShowLocalControlPacket> &Select(FBBBLMGInputState &State, const FBBBLMGShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBLMGInputSlot<FBBBLMGVisibilityRemoteMessagePacket> &Select(FBBBLMGInputState &State, const FBBBLMGVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBLMGInputSlot<FBBBLMGVisibilityAuthorityFactPacket> &Select(FBBBLMGInputState &State, const FBBBLMGVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
