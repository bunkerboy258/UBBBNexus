#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterInput::Initialize(FBBBInputRuntimeData &InData)
{
    Data = &InData;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterControlInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && !Packet.MoveWorld.ContainsNaN()
        && !Packet.FacingWorld.ContainsNaN() && !Packet.AimTargetWorld.ContainsNaN(),
        TEXT("[UBBBC]Invalid control input")))
    {
        return false;
    }
    // 持续状态保留最后一次提交 跳跃边沿不得被同帧后续采样吞掉
    const bool bJump = Data->Control.bJump || Packet.bJump;
    Data->Control = Packet;
    Data->Control.MoveWorld = Packet.MoveWorld.GetClampedToMaxSize(1.0f);
    Data->Control.bJump = bJump;
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterEquipmentInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Equipment input unavailable")))
    {
        return false;
    }
    Data->Pending.Equipment.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBEquipmentActionEvent &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && Packet.Sequence > 0,
        TEXT("[UBBBC]Invalid equipment result")))
    {
        return false;
    }
    Data->Pending.Results.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterReloadAnimationInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && Packet.Sequence > 0,
        TEXT("[UBBBC]Invalid animation input")))
    {
        return false;
    }
    Data->Pending.Notifications.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterMontagePacket &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && IsValid(Packet.Montage)
        && Packet.Sequence > 0 && FMath::IsFinite(Packet.PlayRate) && Packet.PlayRate > 0.0f,
        TEXT("[UBBBC]Invalid montage input")))
    {
        return false;
    }
    Data->Pending.Montages.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBPlayerCameraInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && !Packet.Impulse.ContainsNaN()
        && FMath::IsFinite(Packet.RecoverySpeed) && Packet.RecoverySpeed > 0.0f,
        TEXT("[UBBBC]Invalid camera input")))
    {
        return false;
    }
    Data->Pending.Camera.Add(Packet);
    return true;
}

void FBBBCharacterInput::SubmitRestore(const FBBBCharacterRestoreInput &Packet)
{
    // 还原入口仅向网络系统开放 普通调用者不能自行提升优先级
    if (ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Restore input unavailable")))
    {
        Data->Pending.Restores.Add(Packet);
    }
}
