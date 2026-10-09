#pragma once

#include <type_traits>

#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/RuntimeData/BBBMeleeRuntimeData.h"

struct FBBBMeleeUpdateContext;

/** 固定槽位存储与按序解析 */
class FBBBMeleeParseProcessor final
{
public:
    /** @param Context	本帧依赖 @return 无 */
    static void Update(FBBBMeleeUpdateContext &Context);

    /** @param Data	本类装备的黑板 @return 无 */
    static void Clear(FBBBMeleeRuntimeData &Data);

    /** @param Data	本类装备的黑板 @param Packet	本帧输入 @return 是否接受 */
    template<typename TPacket>
    static bool Submit(FBBBMeleeRuntimeData &Data, TPacket Packet)
    {
        auto &Slot = Select(Data.Parse.InputState, Packet);
        if constexpr (std::is_same_v<TPacket, FBBBMeleeActionPermissionLocalControlPacket>)
        {
            Slot.Packet.Permissions.Append(Packet.Permissions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeBeginActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeBeginContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeEndContactLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeEndActionLocalControlPacket>)
        {
            Slot.Packet.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackStartRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackEndRemoteMessagePacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackStartAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }
        if constexpr (std::is_same_v<TPacket, FBBBMeleeAttackEndAuthorityFactPacket>)
        {
            Slot.Packet.Sequences.Append(Packet.Sequences);
            Slot.Packet.Revisions.Append(Packet.Revisions);
        }

        if constexpr (!(std::is_same_v<TPacket, FBBBMeleeActionPermissionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBMeleeBeginActionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBMeleeBeginContactLocalControlPacket>
            || std::is_same_v<TPacket, FBBBMeleeEndContactLocalControlPacket>
            || std::is_same_v<TPacket, FBBBMeleeEndActionLocalControlPacket>
            || std::is_same_v<TPacket, FBBBMeleeAttackStartRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBMeleeAttackEndRemoteMessagePacket>
            || std::is_same_v<TPacket, FBBBMeleeAttackStartAuthorityFactPacket>
            || std::is_same_v<TPacket, FBBBMeleeAttackEndAuthorityFactPacket>))
        {
            Slot.Packet = MoveTemp(Packet);
        }
        Slot.bActive = true;
        return true;
    }

private:
    static TBBBMeleeInputSlot<FBBBMeleeEquipLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeEquipLocalControlPacket &)
    {
        return State.Equip;
    }

    static TBBBMeleeInputSlot<FBBBMeleeEquipAuthorityFactPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeEquipAuthorityFactPacket &)
    {
        return State.AuthorityEquip;
    }

    static TBBBMeleeInputSlot<FBBBMeleeAttackLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeAttackLocalControlPacket &)
    {
        return State.Attack;
    }

    static TBBBMeleeInputSlot<FBBBMeleeActionPermissionLocalControlPacket> &Select(FBBBMeleeInputState &State, const FBBBMeleeActionPermissionLocalControlPacket &)
    {
        return State.ActionPermission;
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

};
