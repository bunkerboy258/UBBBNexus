#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"

struct FBBBPistolParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBPistolParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBPistolParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBPistolRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBPistolRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBPistolPrimaryLocalControlPacket>)
        {
            if (!Slot.bActive)
            {
                Slot.Packet.PressCount = 0;
            }
            Slot.Packet.PressCount += Packet.PressCount;
        }
        if constexpr (!std::is_same_v<TPacket, FBBBPistolPrimaryLocalControlPacket>)
        {
            if constexpr (std::is_same_v<TPacket, FBBBPistolEndContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolEndActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolBeginContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolBeginActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolFireRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolReloadStartRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolReloadEndRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolFireAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolReloadStartAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolReloadEndAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolVisibilityRemoteMessagePacket>)
            {
                Slot.Packet.Hidden.Append(Packet.Hidden);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBPistolVisibilityAuthorityFactPacket>)
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
    static TBBBPistolInputSlot<FBBBPistolEndContactLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBPistolInputSlot<FBBBPistolEndActionLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBPistolInputSlot<FBBBPistolBeginContactLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBPistolInputSlot<FBBBPistolBeginActionLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
    static TBBBPistolInputSlot<FBBBPistolEquipLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBPistolInputSlot<FBBBPistolEquipAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBPistolInputSlot<FBBBPistolPrimaryLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBPistolInputSlot<FBBBPistolSecondaryLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolSecondaryLocalControlPacket &)
    {
        return State.Reload;
    }

    static TBBBPistolInputSlot<FBBBPistolBlockFireLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolBlockFireLocalControlPacket &)
    {
        return State.BlockFire;
    }

    static TBBBPistolInputSlot<FBBBPistolAllowFireLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolAllowFireLocalControlPacket &)
    {
        return State.AllowFire;
    }

    static TBBBPistolInputSlot<FBBBPistolLoadAmmoLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolLoadAmmoLocalControlPacket &)
    {
        return State.LoadMagazine;
    }

    static TBBBPistolInputSlot<FBBBPistolInterruptReloadLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReload;
    }



    static TBBBPistolInputSlot<FBBBPistolFireRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolFireRemoteMessagePacket &)
    {
        return State.RemoteFire;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadStartRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadStartRemoteMessagePacket &)
    {
        return State.RemoteReloadStart;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadEndRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadEndRemoteMessagePacket &)
    {
        return State.RemoteReloadEnd;
    }

    static TBBBPistolInputSlot<FBBBPistolFireAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolFireAuthorityFactPacket &)
    {
        return State.AuthorityFire;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadStartAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadStartAuthorityFactPacket &)
    {
        return State.AuthorityReloadStart;
    }

    static TBBBPistolInputSlot<FBBBPistolReloadEndAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolReloadEndAuthorityFactPacket &)
    {
        return State.AuthorityReloadEnd;
    }

    static TBBBPistolInputSlot<FBBBPistolHideLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBPistolInputSlot<FBBBPistolShowLocalControlPacket> &Select(FBBBPistolInputState &State, const FBBBPistolShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBPistolInputSlot<FBBBPistolVisibilityRemoteMessagePacket> &Select(FBBBPistolInputState &State, const FBBBPistolVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBPistolInputSlot<FBBBPistolVisibilityAuthorityFactPacket> &Select(FBBBPistolInputState &State, const FBBBPistolVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
