#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputRuntimeData.h"

void FBBBCharacterInput::Initialize(FBBBCharacterInputRuntimeData &InData)
{
    Data = &InData;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterContinuousInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && !Packet.Movement.MoveWorld.ContainsNaN()
        && !Packet.Movement.FacingWorld.ContainsNaN() && !Packet.Aim.AimTargetWorld.ContainsNaN(),
        TEXT("[UBBBC]Invalid continuous input")))
    {
        return false;
    }

    Data->Continuous = Packet;
    Data->Continuous.Movement.MoveWorld = Packet.Movement.MoveWorld.GetClampedToMaxSize(1.0f);
    return true;
}

bool FBBBCharacterInput::Submit(FBBBCharacterDiscreteInput Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Discrete input unavailable")))
    {
        return false;
    }

    if (Packet.Equipment.ActionEvent.ActionType != EBBBCharacterActionType::None
        && !ensureMsgf(Packet.Equipment.ActionEvent.Sequence > 0, TEXT("[UBBBC]Invalid equipment fact")))
    {
        return false;
    }

    if (Packet.Reload.Phase != EBBBCharacterReloadPhase::None
        && !ensureMsgf(Packet.Reload.Sequence > 0, TEXT("[UBBBC]Invalid reload animation input")))
    {
        return false;
    }

    if (Packet.Montage.Montage && !ensureMsgf(FMath::IsFinite(Packet.Montage.PlayRate)
        && Packet.Montage.PlayRate > 0.0f, TEXT("[UBBBC]Invalid montage input")))
    {
        return false;
    }

    if (Packet.Camera.RecoverySpeed > 0.0f && !ensureMsgf(!Packet.Camera.Impulse.ContainsNaN()
        && FMath::IsFinite(Packet.Camera.RecoverySpeed), TEXT("[UBBBC]Invalid camera input")))
    {
        return false;
    }

    Data->PendingDiscrete.Add(MoveTemp(Packet));
    return true;
}

void FBBBCharacterInput::SubmitRestore(FBBBCharacterRestoreDiscreteInput Packet)
{
    if (ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Restore input unavailable")))
    {
        Data->PendingRestoreDiscrete.Add(MoveTemp(Packet));
    }
}
