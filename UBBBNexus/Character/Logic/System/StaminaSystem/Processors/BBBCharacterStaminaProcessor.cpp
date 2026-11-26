#include "BBBWork/UBBBNexus/Character/Logic/System/StaminaSystem/Processors/BBBCharacterStaminaProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/StaminaSystem/DomainData/Context/BBBCharacterStaminaUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"

void FBBBCharacterStaminaProcessor::Update(FBBBCharacterStaminaUpdateContext &Context) const
{
    FBBBCharacterStaminaState &State = Context.Data.Stamina.StaminaState;
    const UBBBCharacterConfig &Config = Context.Config;
    const float Maximum = FMath::Max(Config.MaximumStamina, 0.0f);

    // Mirror 只通过权威事实还原 禁止本地初始化以免挡住同版本结果
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        return;
    }

    if (!State.bInitialized)
    {
        State.Maximum = Maximum;
        State.Current = Maximum;
        State.SecondsSinceDrain = Config.RegenDelaySeconds;
        State.bInitialized = true;
        ++State.Revision;
    }

    if (!FMath::IsNearlyEqual(State.Maximum, Maximum))
    {
        const float Ratio = State.Maximum > 0.0f ? State.Current / State.Maximum : 1.0f;
        State.Maximum = Maximum;
        State.Current = FMath::Clamp(Ratio * Maximum, 0.0f, Maximum);
        ++State.Revision;
    }

    const float DeltaSeconds = FMath::Max(Context.Data.External.ReadWorldState().FrameDeltaSeconds, 0.0f);
    const auto &Locomotion = Context.Data.Locomotion.ReadLocomotionState();
    const auto &Use = Context.Data.Interaction.ReadInteractionUseState();

    float Drain = 0.0f;
    if (Locomotion.bSlide)
    {
        Drain += FMath::Max(Config.SlideDrainPerSecond, 0.0f) * DeltaSeconds;
    }
    else if (Locomotion.bSprint && !Locomotion.MovementInput.IsNearlyZero())
    {
        Drain += FMath::Max(Config.SprintDrainPerSecond, 0.0f) * DeltaSeconds;
    }

    if (Use.Fact.bQuickMelee && Use.Fact.bActive && Use.UseSerial != State.LastQuickMeleeSerial)
    {
        Drain += FMath::Max(Config.MeleeAttackCost, 0.0f);
        State.LastQuickMeleeSerial = Use.UseSerial;
    }

    if (const ABBBMeleeEquipment *Melee = Cast<ABBBMeleeEquipment>(Context.Character.GetHeldEquipment()))
    {
        const int32 AttackSequence = Melee->GetAttackSequence();
        if (State.LastEquippedAttackSequence < 0)
        {
            State.LastEquippedAttackSequence = AttackSequence;
        }
        else if (AttackSequence != State.LastEquippedAttackSequence)
        {
            Drain += FMath::Max(Config.MeleeAttackCost, 0.0f);
            State.LastEquippedAttackSequence = AttackSequence;
        }
    }
    else
    {
        State.LastEquippedAttackSequence = -1;
    }

    bool bChanged = false;
    if (Drain > 0.0f)
    {
        const float Next = FMath::Max(State.Current - Drain, 0.0f);
        bChanged = !FMath::IsNearlyEqual(Next, State.Current);
        State.Current = Next;
        State.SecondsSinceDrain = 0.0f;
    }
    else
    {
        State.SecondsSinceDrain += DeltaSeconds;
        if (State.SecondsSinceDrain >= FMath::Max(Config.RegenDelaySeconds, 0.0f)
            && State.Current < State.Maximum)
        {
            const float Next = FMath::Min(
                State.Current + FMath::Max(Config.RegenPerSecond, 0.0f) * DeltaSeconds,
                State.Maximum);
            bChanged = !FMath::IsNearlyEqual(Next, State.Current);
            State.Current = Next;
        }
    }

    if (bChanged)
    {
        ++State.Revision;
    }
}
