#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"

struct FBBBShotgunParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBShotgunParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBShotgunParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBShotgunRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBShotgunRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBShotgunPrimaryLocalControlPacket>)
        {
            if (!Slot.bActive)
            {
                Slot.Packet.PressCount = 0;
            }
            Slot.Packet.PressCount += Packet.PressCount;
        }
        if constexpr (!std::is_same_v<TPacket, FBBBShotgunPrimaryLocalControlPacket>)
        {
            if constexpr (std::is_same_v<TPacket, FBBBShotgunEndContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunEndActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunBeginContactLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunBeginActionLocalControlPacket>)
            {
                Slot.Packet.Tokens.Append(Packet.Tokens);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunFireRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadStartRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadEndRemoteMessagePacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunFireAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadStartAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunReloadEndAuthorityFactPacket>)
            {
                Slot.Packet.Sequences.Append(Packet.Sequences);
                Slot.Packet.Revisions.Append(Packet.Revisions);
                Slot.Packet.Completed.Append(Packet.Completed);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunVisibilityRemoteMessagePacket>)
            {
                Slot.Packet.Hidden.Append(Packet.Hidden);
                Slot.Packet.Revisions.Append(Packet.Revisions);
            }
            else if constexpr (std::is_same_v<TPacket, FBBBShotgunVisibilityAuthorityFactPacket>)
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
    static TBBBShotgunInputSlot<FBBBShotgunEndContactLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunEndContactLocalControlPacket &)
    {
        return State.EndContactLocalControl;
    }
    static TBBBShotgunInputSlot<FBBBShotgunEndActionLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunEndActionLocalControlPacket &)
    {
        return State.EndActionLocalControl;
    }
    static TBBBShotgunInputSlot<FBBBShotgunBeginContactLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunBeginContactLocalControlPacket &)
    {
        return State.BeginContactLocalControl;
    }
    static TBBBShotgunInputSlot<FBBBShotgunBeginActionLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunBeginActionLocalControlPacket &)
    {
        return State.BeginActionLocalControl;
    }
    static TBBBShotgunInputSlot<FBBBShotgunReloadCycleEndLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunReloadCycleEndLocalControlPacket &)
    {
        return State.ReloadCycleEnd;
    }

    static TBBBShotgunInputSlot<FBBBShotgunEquipLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBShotgunInputSlot<FBBBShotgunEquipAuthorityFactPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBShotgunInputSlot<FBBBShotgunPrimaryLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunPrimaryLocalControlPacket &)
    {
        return State.Fire;
    }

    static TBBBShotgunInputSlot<FBBBShotgunSecondaryLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunSecondaryLocalControlPacket &)
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

    static TBBBShotgunInputSlot<FBBBShotgunHideLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBShotgunInputSlot<FBBBShotgunShowLocalControlPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBShotgunInputSlot<FBBBShotgunVisibilityRemoteMessagePacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBShotgunInputSlot<FBBBShotgunVisibilityAuthorityFactPacket> &Select(FBBBShotgunInputState &State, const FBBBShotgunVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
