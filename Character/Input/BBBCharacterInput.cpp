#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterInput::Initialize(FBBBInputRuntimeData &InData)
{
    Data = &InData;
}

bool FBBBCharacterInput::Submit(const FBBBMoveInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && !Packet.World.ContainsNaN(),
        TEXT("[UBBBC]Invalid move input")))
    {
        return false;
    }
    Data->Continuous.Move = Packet;
    Data->Continuous.Move.World = Packet.World.GetClampedToMaxSize(1.0f);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBViewInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && !Packet.FacingWorld.ContainsNaN()
        && !Packet.AimTargetWorld.ContainsNaN(), TEXT("[UBBBC]Invalid view input")))
    {
        return false;
    }
    Data->Continuous.View = Packet;
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBAimInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Aim input unavailable")))
    {
        return false;
    }
    Data->Continuous.Aim = Packet;
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBWalkInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Walk input unavailable")))
    {
        return false;
    }
    Data->Continuous.Walk = Packet;
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBSprintInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Sprint input unavailable")))
    {
        return false;
    }
    Data->Continuous.Sprint = Packet;
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBCrouchInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Crouch input unavailable")))
    {
        return false;
    }
    Data->Continuous.Crouch = Packet;
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBJumpInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Jump input unavailable")))
    {
        return false;
    }
    Data->Pending.Jump.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBEquipInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Equipment input unavailable")))
    {
        return false;
    }
    Data->Pending.Equip.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBFireInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Fire input unavailable")))
    {
        return false;
    }
    Data->Pending.Fire.Add(Packet);
    return true;
}

bool FBBBCharacterInput::Submit(const FBBBReloadInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Reload input unavailable")))
    {
        return false;
    }
    Data->Pending.Reload.Add(Packet);
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
