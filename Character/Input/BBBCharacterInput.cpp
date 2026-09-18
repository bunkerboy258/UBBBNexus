#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputRuntimeData.h"

void FBBBCharacterInput::Initialize(FBBBCharacterInputRuntimeData &InData)
{
    Data = &InData;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterContinuousInput &Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data && !Packet.MoveWorld.ContainsNaN()
        && !Packet.FacingWorld.ContainsNaN() && !Packet.AimTargetWorld.ContainsNaN(),
        TEXT("[UBBBC]Invalid continuous input")))
    {
        return false;
    }

    Data->Continuous = Packet;
    Data->Continuous.MoveWorld = Packet.MoveWorld.GetClampedToMaxSize(1.0f);
    return true;
}

bool FBBBCharacterInput::Submit(FBBBCharacterDiscreteInput Packet)
{
    if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Discrete input unavailable")))
    {
        return false;
    }

    if (Packet.EquipmentEvent.ActionType != EBBBCharacterActionType::None
        && !ensureMsgf(Packet.EquipmentEvent.Sequence > 0, TEXT("[UBBBC]Invalid equipment fact")))
    {
        return false;
    }

    if (Packet.ReloadPhase != EBBBCharacterReloadPhase::None
        && !ensureMsgf(Packet.Sequence > 0, TEXT("[UBBBC]Invalid reload animation input")))
    {
        return false;
    }

    if (Packet.Montage && !ensureMsgf(FMath::IsFinite(Packet.MontagePlayRate)
        && Packet.MontagePlayRate > 0.0f, TEXT("[UBBBC]Invalid montage input")))
    {
        return false;
    }

    if (Packet.CameraRecoverySpeed > 0.0f && !ensureMsgf(!Packet.CameraImpulse.ContainsNaN()
        && FMath::IsFinite(Packet.CameraRecoverySpeed), TEXT("[UBBBC]Invalid camera input")))
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
