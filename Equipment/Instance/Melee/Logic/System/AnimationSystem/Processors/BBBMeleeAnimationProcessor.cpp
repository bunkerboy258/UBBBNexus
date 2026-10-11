#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/AnimationSystem/Processors/BBBMeleeAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Animation/BBBMeleeAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"

void FBBBMeleeAnimationProcessor::Update(FBBBMeleeUpdateContext &Context)
{
    auto &State = Context.Data.Animation.AnimationState;
    const auto &Action = Context.Data.Action.ReadMeleeActionState();
    UBBBMeleeAnimInstance *Animation = Cast<UBBBMeleeAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("近战装备必须使用 UBBBMeleeAnimInstance")))
    {
        return;
    }

    const auto &Input = Context.Data.Action.ReadMeleeActionInputState();
    if (Input.bEquip && Context.Definition.CharacterEquipMontage)
    {
        if (Context.bCausal)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Context.Definition.CharacterEquipMontage});
        }
        if (!Context.bCausal)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{Context.Definition.CharacterEquipMontage});
        }
    }

    if (!State.bInitialized && !Context.bCausal)
    {
        State.AttackSequence = Action.AttackSequence;
    }
    const bool bStart = Action.bAttacking && (Action.AttackSequence != State.AttackSequence || !State.bAttacking);
    if (bStart)
    {
        if (!Context.bCausal)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{Context.Definition.AttackMontage});
        }
        if (Context.bCausal)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Context.Definition.AttackMontage});
        }
    }
    if (!Action.bAttacking && State.bAttacking)
    {
        UAnimInstance *MainAnimation = Context.Character.GetMesh()->GetAnimInstance();
        const bool bOwnMontage = MainAnimation && MainAnimation->Montage_IsActive(Context.Definition.AttackMontage);
        if (bOwnMontage && !Context.bCausal)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{nullptr});
        }
        if (bOwnMontage && Context.bCausal)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{nullptr});
        }
        Stop(Context);
    }
    Animation->PublishAnimationFacts(FBBBEquipmentAnimationFacts{});
    Animation->AttackSequence = Action.AttackSequence;
    Animation->bAttacking = Action.bAttacking;
    State.AttackSequence = Action.AttackSequence;
    State.bAttacking = Action.bAttacking;
    State.bInitialized = true;
}
void FBBBMeleeAnimationProcessor::Stop(FBBBMeleeUpdateContext &Context)
{
    auto &State = Context.Data.Animation.AnimationState;
    State.bAttacking = false;
    if (auto *Animation = Cast<UBBBMeleeAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance()))
    {
        Animation->bAttacking = false;
    }
}
