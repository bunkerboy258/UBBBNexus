#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterInputState &InputState,
    FBBBCharacterInputContext &Context) const
{
    Context.Control.bJump = false;

    Process(InputState.RescueBeginLocalControl, Context);
    Process(InputState.RescueCancelLocalControl, Context);
    Process(InputState.RescueRequestLocalControl, Context);
    Process(InputState.RescueRequestRemoteMessage, Context);
    Process(InputState.RescueEndLocalControl, Context);
    Process(InputState.RescueEndRemoteMessage, Context);
    Process(InputState.RescueReplyLocalControl, Context);
    Process(InputState.RescueReplyRemoteMessage, Context);
    Process(InputState.RescueSnapshotRemoteMessage, Context);
    Process(InputState.RescueSnapshotAuthorityFact, Context);

    Process(InputState.Damage, Context);
    Process(InputState.AuthorityLife, Context);
    Process(InputState.RemoteLife, Context);
    Process(InputState.DamageDelivery, Context);

    Process(InputState.AimState, Context);
    Process(InputState.FireIntent, Context);
    Process(InputState.RunState, Context);
    Process(InputState.RemoteAcceleration, Context);
    Process(InputState.AuthorityAcceleration, Context);
    Process(InputState.Recovery, Context);
    Process(InputState.UseRemoteMessage, Context);
    Process(InputState.UseAuthorityFact, Context);
    Process(InputState.ItemConsume, Context);
    Process(InputState.ItemAdd, Context);
    Process(InputState.ItemMove, Context);
    Process(InputState.RemoteInteractionSelectionState, Context);
    Process(InputState.AuthorityInteractionSelectionState, Context);
    Process(InputState.ItemSelect, Context);
    Process(InputState.QuickMelee, Context);
    Process(InputState.MeleeBegin, Context);
    Process(InputState.MeleeEnd, Context);
    Process(InputState.ItemQuickUse, Context);
    Process(InputState.InteractionPrimary, Context);
    Process(InputState.InteractionSecondary, Context);
    Process(InputState.InteractionRelease, Context);
    Process(InputState.InteractionFinish, Context);
    Process(InputState.AppearanceColorLocalControl, Context);
    Process(InputState.AppearanceCamouflageLocalControl, Context);
    Process(InputState.AppearanceBaseLocalControl, Context);
    Process(InputState.AppearanceDirtLocalControl, Context);
    Process(InputState.AppearanceWeatheringLocalControl, Context);
    Process(InputState.AppearanceRemoteMessage, Context);
    Process(InputState.AppearanceAuthorityFact, Context);

    Process(InputState.EquipmentShow, Context);
    Process(InputState.EquipmentHide, Context);
    Process(InputState.EquipmentBeginAction, Context);
    Process(InputState.EquipmentEndAction, Context);
    Process(InputState.EquipmentBeginContact, Context);
    Process(InputState.EquipmentEndContact, Context);



    Process(InputState.Movement, Context);
    Process(InputState.Run, Context);
    Process(InputState.Crouch, Context);
    Process(InputState.Aim, Context);

    Process(InputState.Jump, Context);

    Process(InputState.FullBodyMontage, Context);
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
}
