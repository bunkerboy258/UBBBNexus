#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterInputState &InputState,
    FBBBCharacterInputContext &Context) const
{
    Context.Control.bJump = false;

    Process(InputState.AimState, Context);
    Process(InputState.RunState, Context);
    Process(InputState.EquipmentSelectionState, Context);
    Process(InputState.EquipmentSlot, Context);

    Process(InputState.Movement, Context);
    Process(InputState.Run, Context);
    Process(InputState.Crouch, Context);
    Process(InputState.Aim, Context);

    Process(InputState.Jump, Context);

    Process(InputState.FullBodyMontage, Context);
    Process(InputState.UpperBodyMontage, Context);
    Process(InputState.FullBodyAdditivePreAimMontage, Context);
    Process(InputState.UpperBodyAdditiveMontage, Context);
    Process(InputState.AdditiveHitReactMontage, Context);
    Process(InputState.Camera, Context);
}
