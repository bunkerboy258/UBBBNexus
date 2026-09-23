#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterInputState &InputState,
    FBBBCharacterInputContext &Context) const
{
    Context.Events.ActionEvents.Reset();
    Context.Control.bFire = false;
    Context.Control.bJump = false;

    InputState.bProcessing = true;

    Process(InputState.EquipmentState, Context);
    Process(InputState.AimState, Context);
    Process(InputState.LocomotionState, Context);

    Process(InputState.EquipFact, Context);
    Process(InputState.FireFact, Context);
    Process(InputState.ReloadStartedFact, Context);
    Process(InputState.MagazineDetachedFact, Context);
    Process(InputState.MagazineLoadedFact, Context);
    Process(InputState.ReloadCancelledFact, Context);

    Process(InputState.Movement, Context);
    Process(InputState.Aim, Context);

    Process(InputState.EquipSlot, Context);
    if (Context.SelectedEquipment && Context.AnimationInstance)
    {
        Context.AnimationInstance->ClearMontageContributions();
    }

    Process(InputState.Reload, Context);
    Process(InputState.Fire, Context);
    Process(InputState.Jump, Context);

    Process(InputState.FullBodyMontage, Context);
    Process(InputState.UpperBodyMontage, Context);
    Process(InputState.FullBodyAdditivePreAimMontage, Context);
    Process(InputState.UpperBodyAdditiveMontage, Context);
    Process(InputState.AdditiveHitReactMontage, Context);
    Process(InputState.Camera, Context);

    InputState.bProcessing = false;
    FinalizeControl(Context);
}

void FBBBCharacterInputProcessor::FinalizeControl(FBBBCharacterInputContext &Context)
{
    Context.Control.bFire = Context.bFire;
    Context.Control.bSprint = Context.Control.bSprint
        && !Context.Control.bAim
        && !Context.Control.bFire;
}
