#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/RuntimeData/BBBMeleeRuntimeData.h"

struct FBBBMeleeParseUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBMeleeParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBMeleeParseUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBMeleeRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    static void Submit(FBBBMeleeRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBMeleeBeginActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeBeginContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeEndContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeEndActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeVisibilityRemoteMessagePacket>)
        {
            Slot.Packet.Hidden.Append(Packet.Hidden);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        else if constexpr (std::is_same_v<TPacket, FBBBMeleeVisibilityAuthorityFactPacket>)
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
    static TBBBMeleeInputSlot<FBBBMeleeLoadAmmoLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeLoadAmmoLocalControlPacket &)
    {
        return State.LoadAmmoLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeInterruptReloadLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeInterruptReloadLocalControlPacket &)
    {
        return State.InterruptReloadLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeBlockFireLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeBlockFireLocalControlPacket &)
    {
        return State.BlockFireLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeAllowFireLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeAllowFireLocalControlPacket &)
    {
        return State.AllowFireLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeSecondaryLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeSecondaryLocalControlPacket &)
    {
        return State.SecondaryLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeEquipLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBMeleeInputSlot<FBBBMeleeEquipAuthorityFactPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBMeleeInputSlot<FBBBMeleePrimaryLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleePrimaryLocalControlPacket &)
    {
        return State.Attack;
    }



    static TBBBMeleeInputSlot<FBBBMeleeBeginActionLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeBeginActionLocalControlPacket &)
    {
        return State.BeginAction;
    }

    static TBBBMeleeInputSlot<FBBBMeleeBeginContactLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeBeginContactLocalControlPacket &)
    {
        return State.BeginContact;
    }

    static TBBBMeleeInputSlot<FBBBMeleeEndContactLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeEndContactLocalControlPacket &)
    {
        return State.EndContact;
    }

    static TBBBMeleeInputSlot<FBBBMeleeEndActionLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeEndActionLocalControlPacket &)
    {
        return State.EndAction;
    }

    static TBBBMeleeInputSlot<FBBBMeleeAttackStartRemoteMessagePacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeAttackStartRemoteMessagePacket &)
    {
        return State.RemoteAttackStart;
    }

    static TBBBMeleeInputSlot<FBBBMeleeAttackEndRemoteMessagePacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeAttackEndRemoteMessagePacket &)
    {
        return State.RemoteAttackEnd;
    }

    static TBBBMeleeInputSlot<FBBBMeleeAttackStartAuthorityFactPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeAttackStartAuthorityFactPacket &)
    {
        return State.AuthorityAttackStart;
    }

    static TBBBMeleeInputSlot<FBBBMeleeAttackEndAuthorityFactPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeAttackEndAuthorityFactPacket &)
    {
        return State.AuthorityAttackEnd;
    }

    static TBBBMeleeInputSlot<FBBBMeleeHideLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeHideLocalControlPacket &)
    {
        return State.HideLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeShowLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeShowLocalControlPacket &)
    {
        return State.ShowLocalControl;
    }
    static TBBBMeleeInputSlot<FBBBMeleeVisibilityRemoteMessagePacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeVisibilityRemoteMessagePacket &)
    {
        return State.VisibilityRemoteMessage;
    }
    static TBBBMeleeInputSlot<FBBBMeleeVisibilityAuthorityFactPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeVisibilityAuthorityFactPacket &)
    {
        return State.VisibilityAuthorityFact;
    }
};
