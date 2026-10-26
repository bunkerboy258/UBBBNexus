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
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterFireIntentLocalControlPacket, FireIntent)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterLifeAuthorityFactPacket, AuthorityLife)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterLifeRemoteMessagePacket, RemoteLife)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterDamageDeliveryRemoteMessagePacket, DamageDelivery)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterDamageLocalControlPacket, Damage)
BBB_CHARACTER_INPUT_SLOT(FBBBRunStateAuthorityFactPacket, RunState)
BBB_CHARACTER_INPUT_SLOT(FBBBAccelerationRemoteMessagePacket, RemoteAcceleration)
BBB_CHARACTER_INPUT_SLOT(FBBBAccelerationAuthorityFactPacket, AuthorityAcceleration)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterItemAddLocalControlPacket, ItemAdd)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterItemMoveLocalControlPacket, ItemMove)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionSelectionAuthorityFactPacket, AuthorityInteractionSelectionState)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionSelectionRemoteMessagePacket, RemoteInteractionSelectionState)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterItemSelectLocalControlPacket, ItemSelect)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterMovementLocalControlPacket, Movement)
BBB_CHARACTER_INPUT_SLOT(FBBBRunLocalControlPacket, Run)
BBB_CHARACTER_INPUT_SLOT(FBBBCrouchLocalControlPacket, Crouch)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAimLocalControlPacket, Aim)
BBB_CHARACTER_INPUT_SLOT(FBBBJumpLocalControlPacket, Jump)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyMontageLocalControlPacket, FullBodyMontage)
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





BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionBeginActionLocalControlPacket, EquipmentBeginAction)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionEndActionLocalControlPacket, EquipmentEndAction)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionBeginContactLocalControlPacket, EquipmentBeginContact)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionEndContactLocalControlPacket, EquipmentEndContact)

BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueBeginLocalControlPacket, RescueBeginLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueCompleteLocalControlPacket, RescueCompleteLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueCancelLocalControlPacket, RescueCancelLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueRequestLocalControlPacket, RescueRequestLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueRequestRemoteMessagePacket, RescueRequestRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueEndLocalControlPacket, RescueEndLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueEndRemoteMessagePacket, RescueEndRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueReplyLocalControlPacket, RescueReplyLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueReplyRemoteMessagePacket, RescueReplyRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueSnapshotRemoteMessagePacket, RescueSnapshotRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRescueSnapshotAuthorityFactPacket, RescueSnapshotAuthorityFact)

BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceColorLocalControlPacket, AppearanceColorLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceCamouflageLocalControlPacket, AppearanceCamouflageLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceBaseLocalControlPacket, AppearanceBaseLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceDirtLocalControlPacket, AppearanceDirtLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceWeatheringLocalControlPacket, AppearanceWeatheringLocalControl)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceRemoteMessagePacket, AppearanceRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAppearanceAuthorityFactPacket, AppearanceAuthorityFact)

BBB_CHARACTER_INPUT_SLOT(FBBBCharacterItemConsumeLocalControlPacket, ItemConsume)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterItemQuickUseLocalControlPacket, ItemQuickUse)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionPrimaryLocalControlPacket, InteractionPrimary)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionSecondaryLocalControlPacket, InteractionSecondary)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionReleaseLocalControlPacket, InteractionRelease)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterInteractionFinishLocalControlPacket, InteractionFinish)

BBB_CHARACTER_INPUT_SLOT(FBBBCharacterRecoveryLocalControlPacket, Recovery)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterUseRemoteMessagePacket, UseRemoteMessage)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterUseAuthorityFactPacket, UseAuthorityFact)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterQuickMeleeLocalControlPacket, QuickMelee)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterMeleeBeginLocalControlPacket, MeleeBegin)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterMeleeEndLocalControlPacket, MeleeEnd)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentShowLocalControlPacket, EquipmentShow)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterEquipmentHideLocalControlPacket, EquipmentHide)
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
    void Submit(FBBBCharacterInputState &State, TPacket &&Packet)
    {

        using FPacket = typename TDecay<TPacket>::Type;

        TBBBCharacterInputSlot<FPacket> &Slot = TBBBCharacterInputSlotSelector<FPacket>::Get(State);
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionPrimaryLocalControlPacket>)
        {
            if (!Slot.bActive)
            {
                Slot.Data.PressCount = 0;
            }
            Slot.Data.PressCount += Packet.PressCount;
        }
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
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionSelectionAuthorityFactPacket>
            || std::is_same_v<FPacket, FBBBCharacterInteractionSelectionRemoteMessagePacket>)
        {
            Slot.Data.ItemIds.Append(Packet.ItemIds);
            Slot.Data.Generations.Append(Packet.Generations);
        }

        if constexpr (std::is_same_v<FPacket, FBBBCharacterItemAddLocalControlPacket>)
        {
            Slot.Data.ItemIds.Append(Packet.ItemIds);
            Slot.Data.Quantities.Append(Packet.Quantities);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterItemMoveLocalControlPacket>)
        {
            Slot.Data.Sources.Append(Packet.Sources);
            Slot.Data.Targets.Append(Packet.Targets);
            Slot.Data.Instances.Append(Packet.Instances);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterItemSelectLocalControlPacket>)
        {
            Slot.Data.Slots.Append(Packet.Slots);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterEquipmentShowLocalControlPacket>
            || std::is_same_v<FPacket, FBBBCharacterEquipmentHideLocalControlPacket>)
        {
            Slot.Data.Generations.Append(Packet.Generations);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionBeginActionLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionEndActionLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionBeginContactLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionEndContactLocalControlPacket>)
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
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceColorLocalControlPacket>)
        {
            Slot.Data.Instances.Append(Packet.Instances);
            Slot.Data.Slots.Append(Packet.Slots);
            Slot.Data.Colors.Append(Packet.Colors);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceCamouflageLocalControlPacket>)
        {
            Slot.Data.Instances.Append(Packet.Instances);
            Slot.Data.Slots.Append(Packet.Slots);
            Slot.Data.Values.Append(Packet.Values);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceBaseLocalControlPacket>)
        {
            Slot.Data.Slots.Append(Packet.Slots);
            Slot.Data.Resources.Append(Packet.Resources);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceDirtLocalControlPacket>)
        {
            Slot.Data.Values.Append(Packet.Values);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceWeatheringLocalControlPacket>)
        {
            Slot.Data.Values.Append(Packet.Values);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceRemoteMessagePacket>)
        {
            Slot.Data.Snapshots.Append(Packet.Snapshots);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterAppearanceAuthorityFactPacket>)
        {
            Slot.Data.Snapshots.Append(Packet.Snapshots);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterItemConsumeLocalControlPacket>)
        {
            Slot.Data.Instances.Append(Packet.Instances);
            Slot.Data.Quantities.Append(Packet.Quantities);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionReleaseLocalControlPacket>
            || std::is_same_v<FPacket, FBBBCharacterInteractionFinishLocalControlPacket>)
        {
            Slot.Data.Recipients.Append(Packet.Recipients);
            Slot.Data.Generations.Append(Packet.Generations);
            Slot.Data.Tokens.Append(Packet.Tokens);
            if constexpr (std::is_same_v<FPacket, FBBBCharacterInteractionFinishLocalControlPacket>)
            {
                Slot.Data.Instances.Append(Packet.Instances);
            }
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterRecoveryLocalControlPacket>)
        {
            Slot.Data.Amounts.Append(Packet.Amounts);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterUseRemoteMessagePacket>)
        {
            Slot.Data.Snapshots.Append(Packet.Snapshots);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterUseAuthorityFactPacket>)
        {
            Slot.Data.Snapshots.Append(Packet.Snapshots);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterMeleeBeginLocalControlPacket>)
        {
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (std::is_same_v<FPacket, FBBBCharacterMeleeEndLocalControlPacket>)
        {
            Slot.Data.Tokens.Append(Packet.Tokens);
        }
        if constexpr (!std::is_same_v<FPacket, FBBBCharacterInteractionPrimaryLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterMeleeEndLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterMeleeBeginLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterUseAuthorityFactPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterUseRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRecoveryLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterItemConsumeLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionReleaseLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionFinishLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionSelectionAuthorityFactPacket> &&
                      !std::is_same_v<FPacket, FBBBAccelerationRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBAccelerationAuthorityFactPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterLifeAuthorityFactPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterLifeRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterDamageDeliveryRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterDamageLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionSelectionRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterItemAddLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterItemMoveLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterItemSelectLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterEquipmentShowLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterEquipmentHideLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionBeginActionLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionEndActionLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionBeginContactLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterInteractionEndContactLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueRequestLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueRequestRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueEndLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueEndRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueReplyLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueReplyRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueSnapshotRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterRescueSnapshotAuthorityFactPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceColorLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceCamouflageLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceBaseLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceDirtLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceWeatheringLocalControlPacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceRemoteMessagePacket> &&
                      !std::is_same_v<FPacket, FBBBCharacterAppearanceAuthorityFactPacket>)
        {
            Slot.Data = Forward<TPacket>(Packet);
        }
        Slot.bActive = true;
        return;
    }
}
