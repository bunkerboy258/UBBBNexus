#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/Processors/BBBMeleeAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Animation/BBBMeleeAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/DomainData/Context/BBBMeleeAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/World.h"

void FBBBMeleeAnimationProcessor::Update(FBBBMeleeAnimationUpdateContext &Context)
{
    auto &State = Context.RuntimeData.Animation.AnimationState;
    const auto &Action = Context.RuntimeData.Action.ReadMeleeActionState();
    UBBBMeleeAnimInstance *Animation = Cast<UBBBMeleeAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("近战装备必须使用 UBBBMeleeAnimInstance")))
    {
        return;
    }

    const auto &Input = Context.RuntimeData.Action.ReadMeleeActionInputState();
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
        Stop(Context);
    }
    Animation->PublishAnimationFacts(FBBBEquipmentAnimationFacts{});
    Animation->AttackSequence = Action.AttackSequence;
    Animation->bAttacking = Action.bAttacking;
    State.AttackSequence = Action.AttackSequence;
    State.bAttacking = Action.bAttacking;
    State.bInitialized = true;
}
void FBBBMeleeAnimationProcessor::Stop(FBBBMeleeAnimationUpdateContext &Context)
{
    auto &State = Context.RuntimeData.Animation.AnimationState;
    State.bAttacking = false;
    if (auto *Animation = Cast<UBBBMeleeAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance()))
    {
        Animation->bAttacking = false;
    }
}
