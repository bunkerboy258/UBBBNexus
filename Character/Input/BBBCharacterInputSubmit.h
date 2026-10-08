#pragma once

#include "CoreMinimal.h"
#include <type_traits>
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterInputState.h"

/** 角色输入包到固定槽位的编译期映射 */
template<typename TPacket>
struct TBBBCharacterInputSlotSelector;

#define BBB_CHARACTER_INPUT_SLOT(PacketType, MemberName) \
    template<> \
    struct TBBBCharacterInputSlotSelector<PacketType> final \
    { \
        static TBBBCharacterInputSlot<PacketType> &Get(FBBBCharacterInputState &State) \
        { \
            return State.MemberName; \
        } \
    };

BBB_CHARACTER_INPUT_SLOT(FBBBAimStateAuthorityFactPacket, AimState)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterLifeAuthorityFactPacket, AuthorityLife)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterLifeRemoteMessagePacket, RemoteLife)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterDamageDeliveryRemoteMessagePacket, DamageDelivery)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentUseAuthorityFactPacket, AuthorityFactEquipmentUse)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentUseRemoteMessagePacket, RemoteMessageEquipmentUse)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterDamageLocalControlPacket, Damage)
BBB_CHARACTER_INPUT_SLOT(FBBBRunStateAuthorityFactPacket, RunState)
BBB_CHARACTER_INPUT_SLOT(FBBBAccelerationRemoteMessagePacket, RemoteAcceleration)
BBB_CHARACTER_INPUT_SLOT(FBBBAccelerationAuthorityFactPacket, AuthorityAcceleration)
BBB_CHARACTER_INPUT_SLOT(FBBBItemAddLocalControlPacket, ItemAdd)
BBB_CHARACTER_INPUT_SLOT(FBBBItemMoveLocalControlPacket, ItemMove)
BBB_CHARACTER_INPUT_SLOT(FBBBEquipmentSelectionAuthorityFactPacket, AuthorityEquipmentSelectionState)
BBB_CHARACTER_INPUT_SLOT(FBBBEquipmentSelectionRemoteMessagePacket, RemoteEquipmentSelectionState)
BBB_CHARACTER_INPUT_SLOT(FBBBItemSelectLocalControlPacket, ItemSelect)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterMovementLocalControlPacket, Movement)
BBB_CHARACTER_INPUT_SLOT(FBBBRunLocalControlPacket, Run)
BBB_CHARACTER_INPUT_SLOT(FBBBCrouchLocalControlPacket, Crouch)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAimLocalControlPacket, Aim)
BBB_CHARACTER_INPUT_SLOT(FBBBJumpLocalControlPacket, Jump)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyMontageLocalControlPacket, FullBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBTraversalMontageLocalControlPacket, TraversalMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyMontageAuthorityFactPacket, AuthorityFullBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyMontageLocalControlPacket, UpperBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyMontageAuthorityFactPacket, AuthorityUpperBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyAdditivePreAimMontageLocalControlPacket, FullBodyAdditivePreAimMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket, AuthorityFullBodyAdditivePreAimMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyAdditiveMontageLocalControlPacket, UpperBodyAdditiveMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyAdditiveMontageAuthorityFactPacket, AuthorityUpperBodyAdditiveMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBAdditiveHitReactMontageLocalControlPacket, AdditiveHitReactMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBAdditiveHitReactMontageAuthorityFactPacket, AuthorityAdditiveHitReactMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBCameraLocalControlPacket, Camera)
BBB_CHARACTER_INPUT_SLOT(FBBBAimImpulseLocalControlPacket, AimImpulse)
BBB_CHARACTER_INPUT_SLOT(FBBBAimImpulseAuthorityFactPacket, AuthorityAimImpulse)

BBB_CHARACTER_INPUT_SLOT(FBBBTraversalStartRemoteMessagePacket, TraversalStartRemoteMessage)

BBB_CHARACTER_INPUT_SLOT(FBBBTraversalEndRemoteMessagePacket, TraversalEndRemoteMessage)

BBB_CHARACTER_INPUT_SLOT(FBBBTraversalStartAuthorityFactPacket, TraversalStartAuthorityFact)

BBB_CHARACTER_INPUT_SLOT(FBBBTraversalEndAuthorityFactPacket, TraversalEndAuthorityFact)

BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentBeginActionLocalControlPacket, EquipmentBeginAction)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentEndActionLocalControlPacket, EquipmentEndAction)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentBeginContactLocalControlPacket, EquipmentBeginContact)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentEndContactLocalControlPacket, EquipmentEndContact)

BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueBeginLocalControlPacket, RescueBeginLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueCancelLocalControlPacket, RescueCancelLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueRequestLocalControlPacket, RescueRequestLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueRequestRemoteMessagePacket, RescueRequestRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueEndLocalControlPacket, RescueEndLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueEndRemoteMessagePacket, RescueEndRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueReplyLocalControlPacket, RescueReplyLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueReplyRemoteMessagePacket, RescueReplyRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueSnapshotRemoteMessagePacket, RescueSnapshotRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueSnapshotAuthorityFactPacket, RescueSnapshotAuthorityFact)

#undef BBB_CHARACTER_INPUT_SLOT

/** 角色固定输入槽位的唯一提交闸口 */
namespace BBBCharacterInput
{
    /**
     * 根据包类型把它放进对应固定槽位
     * @param State 角色固定输入状态
     * @param Packet 输入包
     * @return 是否接受输入
     */
    template<typename TPacket>
    bool Submit(FBBBCharacterInputState &State, TPacket &&Packet)
    {
        if (!ensureMsgf(Packet.IsValid(), TEXT("角色输入包数据无效")))
        {
            return false;
        }

        using FPacket = typename TDecay<TPacket>::Type;

        TBBBCharacterInputSlot<FPacket> &Slot = TBBBCharacterInputSlotSelector<FPacket>::Get(State);
        if constexpr (std::is_same_v<FPacket, FBBBAccelerationRemoteMessagePacket>
            || std::is_same_v<FPacket, FBBBAccelerationAuthorityFactPacket>)
        {
            Slot.Data.Revisions.Append(Packet.Revisions);
            Slot.Data.Accelerations.Append(Packet.Accelerations);
            Slot.Data.MovementInputs.Append(Packet.MovementInputs);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterLifeAuthorityFactPacket>
            || std::is_same_v<FPacket, FBBBCharacterLifeRemoteMessagePacket>)
        {
            Slot.Data.Phases.Append(Packet.Phases);
            Slot.Data.Healths.Append(Packet.Healths);
            Slot.Data.Revisions.Append(Packet.Revisions);
            Slot.Data.HitSerials.Append(Packet.HitSerials);
            Slot.Data.Bones.Append(Packet.Bones);
            Slot.Data.Positions.Append(Packet.Positions);
            Slot.Data.Directions.Append(Packet.Directions);
            Slot.Data.DownedRevisions.Append(Packet.DownedRevisions);
            Slot.Data.RecoveryCrouched.Append(Packet.RecoveryCrouched);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterEquipmentUseAuthorityFactPacket>
            || std::is_same_v<FPacket, FBBBCharacterEquipmentUseRemoteMessagePacket>)
        {
            Slot.Data.Generations.Append(Packet.Generations);
            Slot.Data.Revisions.Append(Packet.Revisions);
            Slot.Data.Usable.Append(Packet.Usable);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterDamageLocalControlPacket>
            || std::is_same_v<FPacket, FBBBCharacterDamageDeliveryRemoteMessagePacket>)
        {
            Slot.Data.Damages.Append(Packet.Damages);
            Slot.Data.Bones.Append(Packet.Bones);
            Slot.Data.Positions.Append(Packet.Positions);
            Slot.Data.Directions.Append(Packet.Directions);
            Slot.Data.Sources.Append(Packet.Sources);
            if constexpr (std::is_same_v<FPacket, FBBBCharacterDamageDeliveryRemoteMessagePacket>)
            {
                Slot.Data.Sequences.Append(Packet.Sequences);
            }
        }
        // 保留同帧全部交接结果 输入解析再按动作标识拒绝过期结果
        if constexpr (std::is_same_v<FPacket, FBBBTraversalStartRemoteMessagePacket>
            || std::is_same_v<FPacket, FBBBTraversalStartAuthorityFactPacket>)
        {
            Slot.Data.ActionIds.Append(Packet.ActionIds);
            Slot.Data.Actions.Append(Packet.Actions);
            Slot.Data.Contacts.Append(Packet.Contacts);
            Slot.Data.Ends.Append(Packet.Ends);
            Slot.Data.Positions.Append(Packet.Positions);
        }
        if constexpr (std::is_same_v<FPacket, FBBBTraversalEndRemoteMessagePacket>
            || std::is_same_v<FPacket, FBBBTraversalEndAuthorityFactPacket>)
        {
            Slot.Data.ActionIds.Append(Packet.ActionIds);
            Slot.Data.ExitVelocities.Append(Packet.ExitVelocities);
        }
        if constexpr (std::is_same_v<FPacket, FBBBEquipmentSelectionAuthorityFactPacket>
            || std::is_same_v<FPacket, FBBBEquipmentSelectionRemoteMessagePacket>)
        {
            Slot.Data.EquipmentIds.Append(Packet.EquipmentIds);
            Slot.Data.Generations.Append(Packet.Generations);
        }

        if constexpr (std::is_same_v<FPacket, FBBBItemAddLocalControlPacket>)
        {
            Slot.Data.EquipmentIds.Append(Packet.EquipmentIds);
        }
        if constexpr (std::is_same_v<FPacket, FBBBItemMoveLocalControlPacket>)
        {
            Slot.Data.Sources.Append(Packet.Sources);
            Slot.Data.Targets.Append(Packet.Targets);
        }
        if constexpr (std::is_same_v<FPacket, FBBBItemSelectLocalControlPacket>)
        {
            Slot.Data.Slots.Append(Packet.Slots);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterEquipmentBeginActionLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterEquipmentEndActionLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterEquipmentBeginContactLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterEquipmentEndContactLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueRequestLocalControlPacket>)
        {
            Slot.Data.RequestSources.Append(Packet.RequestSources);
            Slot.Data.RequestOperations.Append(Packet.RequestOperations);
            Slot.Data.RequestRounds.Append(Packet.RequestRounds);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueRequestRemoteMessagePacket>)
        {
            Slot.Data.RequestSources.Append(Packet.RequestSources);
            Slot.Data.RequestOperations.Append(Packet.RequestOperations);
            Slot.Data.RequestRounds.Append(Packet.RequestRounds);
            Slot.Data.RequestTargets.Append(Packet.RequestTargets);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueEndLocalControlPacket>)
        {
            Slot.Data.CancelSources.Append(Packet.CancelSources);
            Slot.Data.CancelOperations.Append(Packet.CancelOperations);
            Slot.Data.CancelRounds.Append(Packet.CancelRounds);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueEndRemoteMessagePacket>)
        {
            Slot.Data.CancelSources.Append(Packet.CancelSources);
            Slot.Data.CancelOperations.Append(Packet.CancelOperations);
            Slot.Data.CancelRounds.Append(Packet.CancelRounds);
            Slot.Data.CancelTargets.Append(Packet.CancelTargets);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueReplyLocalControlPacket>)
        {
            Slot.Data.ReplySources.Append(Packet.ReplySources);
            Slot.Data.ReplyOperations.Append(Packet.ReplyOperations);
            Slot.Data.ReplyRounds.Append(Packet.ReplyRounds);
            Slot.Data.ReplyRevisions.Append(Packet.ReplyRevisions);
            Slot.Data.ReplyActive.Append(Packet.ReplyActive);
            Slot.Data.ReplyDurations.Append(Packet.ReplyDurations);
            Slot.Data.ReplyReasons.Append(Packet.ReplyReasons);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueReplyRemoteMessagePacket>)
        {
            Slot.Data.ReplySources.Append(Packet.ReplySources);
            Slot.Data.ReplyOperations.Append(Packet.ReplyOperations);
            Slot.Data.ReplyRounds.Append(Packet.ReplyRounds);
            Slot.Data.ReplyRevisions.Append(Packet.ReplyRevisions);
            Slot.Data.ReplyActive.Append(Packet.ReplyActive);
            Slot.Data.ReplyDurations.Append(Packet.ReplyDurations);
            Slot.Data.ReplyReasons.Append(Packet.ReplyReasons);
            Slot.Data.ReplyTargets.Append(Packet.ReplyTargets);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueSnapshotRemoteMessagePacket>)
        {
            Slot.Data.SnapshotPartners.Append(Packet.SnapshotPartners);
            Slot.Data.SnapshotOperations.Append(Packet.SnapshotOperations);
            Slot.Data.SnapshotRounds.Append(Packet.SnapshotRounds);
            Slot.Data.SnapshotRevisions.Append(Packet.SnapshotRevisions);
            Slot.Data.SnapshotHelping.Append(Packet.SnapshotHelping);
            Slot.Data.SnapshotReceiving.Append(Packet.SnapshotReceiving);
            Slot.Data.SnapshotAccepted.Append(Packet.SnapshotAccepted);
            Slot.Data.SnapshotElapsed.Append(Packet.SnapshotElapsed);
            Slot.Data.SnapshotDurations.Append(Packet.SnapshotDurations);
            Slot.Data.SnapshotReasons.Append(Packet.SnapshotReasons);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRescueSnapshotAuthorityFactPacket>)
        {
            Slot.Data.SnapshotPartners.Append(Packet.SnapshotPartners);
            Slot.Data.SnapshotOperations.Append(Packet.SnapshotOperations);
            Slot.Data.SnapshotRounds.Append(Packet.SnapshotRounds);
            Slot.Data.SnapshotRevisions.Append(Packet.SnapshotRevisions);
            Slot.Data.SnapshotHelping.Append(Packet.SnapshotHelping);
            Slot.Data.SnapshotReceiving.Append(Packet.SnapshotReceiving);
            Slot.Data.SnapshotAccepted.Append(Packet.SnapshotAccepted);
            Slot.Data.SnapshotElapsed.Append(Packet.SnapshotElapsed);
            Slot.Data.SnapshotDurations.Append(Packet.SnapshotDurations);
            Slot.Data.SnapshotReasons.Append(Packet.SnapshotReasons);
        }
        if constexpr (!std::is_same_v<FPacket, FBBBEquipmentSelectionAuthorityFactPacket>
            && !std::is_same_v<FPacket, FBBBAccelerationRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBAccelerationAuthorityFactPacket>
            && !std::is_same_v<FPacket, FBBBCharacterLifeAuthorityFactPacket>
            && !std::is_same_v<FPacket, FBBBCharacterLifeRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterDamageDeliveryRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterEquipmentUseAuthorityFactPacket>
            && !std::is_same_v<FPacket, FBBBCharacterEquipmentUseRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterDamageLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBTraversalStartRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBTraversalStartAuthorityFactPacket>
            && !std::is_same_v<FPacket, FBBBTraversalEndRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBTraversalEndAuthorityFactPacket>
            && !std::is_same_v<FPacket, FBBBEquipmentSelectionRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBItemAddLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBItemMoveLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBItemSelectLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterEquipmentBeginActionLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterEquipmentEndActionLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterEquipmentBeginContactLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterEquipmentEndContactLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueRequestLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueRequestRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueEndLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueEndRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueReplyLocalControlPacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueReplyRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueSnapshotRemoteMessagePacket>
            && !std::is_same_v<FPacket, FBBBCharacterRescueSnapshotAuthorityFactPacket>)
        {
            Slot.Data = Forward<TPacket>(Packet);
        }
        Slot.bActive = true;
        return true;
    }
}
