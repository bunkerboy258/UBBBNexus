#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterAnimationInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBCharacterReloadAnimationInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    const FBBBCharacterOperationState &Operation = Data.Operation;
    if (Sequence != Operation.ReloadSequence || Operation.ReloadSequence <= 0 || Operation.bEndQueued)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[UBBBC]Ignored stale animation input Sequence=%d"), Sequence);
        return false;
    }
    if (Phase == EBBBCharacterReloadAnimationPhase::Start && Operation.bMagazineDetached)
    {
        return false;
    }
    if (Phase == EBBBCharacterReloadAnimationPhase::End && !Operation.bMagazineDetached)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload load arrived before detach Sequence=%d"), Sequence);
        return false;
    }
    return true;
}

void FBBBCharacterReloadAnimationInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    FBBBCharacterOperationState &Operation = Data.Operation;
    if (Phase == EBBBCharacterReloadAnimationPhase::Start)
    {
        Operation.bMagazineDetached = true;
    }
    else
    {
        Operation.bEndQueued = true;
        if (Phase == EBBBCharacterReloadAnimationPhase::Interrupted)
        {
            Operation.CancelReloadSequence = Sequence;
        }
    }
    Data.Equipment.Commands.ReloadInputs.Add(*this);
}
