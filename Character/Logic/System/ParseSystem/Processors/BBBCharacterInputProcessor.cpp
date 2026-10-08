#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterInputState &InputState,
    FBBBCharacterInputContext &Context) const
{
    Context.Control.bJump = false;

    Process(InputState.Damage, Context);
    Process(InputState.AuthorityLife, Context);
    Process(InputState.RemoteLife, Context);
    Process(InputState.DamageDelivery, Context);

    Process(InputState.AimState, Context);
    Process(InputState.RunState, Context);
    Process(InputState.RemoteAcceleration, Context);
    Process(InputState.AuthorityAcceleration, Context);
    Process(InputState.ItemAdd, Context);
    Process(InputState.ItemMove, Context);
    Process(InputState.RemoteEquipmentSelectionState, Context);
    Process(InputState.AuthorityEquipmentSelectionState, Context);
    Process(InputState.RemoteMessageEquipmentUse, Context);
    Process(InputState.AuthorityFactEquipmentUse, Context);
    Process(InputState.ItemSelect, Context);
    Process(InputState.EquipmentBeginAction, Context);
    Process(InputState.EquipmentEndAction, Context);
    Process(InputState.EquipmentBeginContact, Context);
    Process(InputState.EquipmentEndContact, Context);


    Process(InputState.Movement, Context);
    Process(InputState.Run, Context);
    Process(InputState.Crouch, Context);
    Process(InputState.Aim, Context);

    Process(InputState.TraversalStartRemoteMessage, Context);
    Process(InputState.TraversalEndRemoteMessage, Context);
    Process(InputState.TraversalStartAuthorityFact, Context);
    Process(InputState.TraversalEndAuthorityFact, Context);
    Process(InputState.Jump, Context);

    Process(InputState.FullBodyMontage, Context);
    Process(InputState.TraversalMontage, Context);
    Process(InputState.AuthorityFullBodyMontage, Context);
    Process(InputState.UpperBodyMontage, Context);
    Process(InputState.AuthorityUpperBodyMontage, Context);
    Process(InputState.FullBodyAdditivePreAimMontage, Context);
    Process(InputState.AuthorityFullBodyAdditivePreAimMontage, Context);
    Process(InputState.UpperBodyAdditiveMontage, Context);
    Process(InputState.AuthorityUpperBodyAdditiveMontage, Context);
    Process(InputState.AdditiveHitReactMontage, Context);
    Process(InputState.AuthorityAdditiveHitReactMontage, Context);
    Process(InputState.Camera, Context);
    Process(InputState.AimImpulse, Context);
    Process(InputState.AuthorityAimImpulse, Context);
}
