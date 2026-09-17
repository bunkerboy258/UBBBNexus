#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBExecutionPipeline::Update() const
{
    if (!ensureMsgf(Data, TEXT("[UBBBC]Execution pipeline is uninitialized")))
    {
        return;
    }
    FBBBCharacterOperationState &Operation = Data->Operation;
    FBBBInputBatch &Frame = Data->Input.Frame;
    FBBBAnimationRuntimeData &Animation = Data->Animation;
    FBBBCharacterEquipmentCommands &Commands = Data->Equipment.Commands;

    // 最高优先级还原只覆盖黑板 不执行装备接口或动画引擎函数
    for (const FBBBCharacterRestoreInput &Restore : Frame.Restores)
    {
        if (!Operation.bRestoreMode)
        {
            continue;
        }
        if (Restore.bEquipmentChanged)
        {
            Commands.PendingRestoredEquipment = Restore.Equipment;
        }
        for (const FBBBEquipmentActionEvent &Action : Restore.Actions)
        {
            Commands.SubmitRestoredAction(Action);
        }
        if (Restore.Aim.IsSet())
        {
            Data->Aim.ApplyRestoredState(Restore.Aim.GetValue());
        }
        if (Restore.Gait.IsSet())
        {
            Data->Locomotion.CommitGait(Restore.Gait.GetValue());
        }
    }

    for (const FBBBEquipmentActionEvent &Result : Frame.Results)
    {
        Data->Equipment.Events.AddAction(Result);
    }
    if (!Operation.bRestoreMode)
    {
        Data->Control.Value = Operation.Control;
        Commands.ReloadInputs = Frame.Notifications;
        if (Operation.SelectedEquipment)
        {
            Data->Equipment.Equipment.DesiredMainHandInstance = Operation.SelectedEquipment;
        }
        if (Operation.bFire)
        {
            Commands.SubmitFire();
        }
        if (Operation.bReload)
        {
            Commands.SubmitReload();
        }
        Data->CameraContributions = Frame.Camera;
    }

    if (Animation.Slots.IsEmpty())
    {
        for (const FName Slot : {FName(TEXT("FullBody")), FName(TEXT("UpperBody")),
            FName(TEXT("FullBodyAdditivePreAim")), FName(TEXT("UpperBodyAdditive")),
            FName(TEXT("AdditiveHitReact"))})
        {
            FBBBCharacterMontageSlotState &State = Animation.Slots.AddDefaulted_GetRef();
            State.Slot = Slot;
        }
    }
    for (FBBBCharacterMontageSlotState &Slot : Animation.Slots)
    {
        if (Operation.SelectedEquipment
            || (Operation.CancelReloadSequence > 0 && Slot.Desired.bReload
                && Slot.Desired.Sequence == Operation.CancelReloadSequence))
        {
            Slot.Desired = FBBBCharacterMontagePacket();
            Slot.Revision = 0;
        }
    }

    for (const FBBBCharacterMontagePacket &Packet : Frame.Montages)
    {
        if (Operation.SelectedEquipment || !IsValid(Packet.Montage))
        {
            continue;
        }
        if (Packet.bReload && (Packet.Sequence == Operation.CancelReloadSequence
            || (!Operation.bRestoreMode && Packet.Sequence != Operation.ReloadSequence)))
        {
            continue;
        }
        UAnimMontage &Montage = *Packet.Montage;
        bool bValidSlots = !Montage.SlotAnimTracks.IsEmpty();
        for (const FSlotAnimationTrack &Track : Montage.SlotAnimTracks)
        {
            bValidSlots &= Animation.Slots.ContainsByPredicate([&Track](const FBBBCharacterMontageSlotState &Slot)
            {
                return Slot.Slot == Track.SlotName;
            });
        }
        if (!ensureMsgf(bValidSlots, TEXT("[UBBBC]Montage uses unsupported slots Asset=%s"), *Montage.GetPathName()))
        {
            continue;
        }

        // 保留引擎同组互斥规则 一次覆盖该组旧期望 再为新蒙太奇全部轨道写入同一修订号
        for (FBBBCharacterMontageSlotState &Slot : Animation.Slots)
        {
            if (Slot.Desired.Montage && Slot.Desired.Montage->GetGroupName() == Montage.GetGroupName())
            {
                Slot.Desired = FBBBCharacterMontagePacket();
                Slot.Revision = 0;
            }
        }
        const uint64 Revision = Animation.NextRevision++;
        for (const FSlotAnimationTrack &Track : Montage.SlotAnimTracks)
        {
            for (FBBBCharacterMontageSlotState &Slot : Animation.Slots)
            {
                if (Slot.Slot == Track.SlotName)
                {
                    Slot.Desired = Packet;
                    Slot.Revision = Revision;
                }
            }
        }
    }
}
