#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBMeleeActionProcessor::Stop(FBBBMeleeRuntimeData &Data)
{
    auto &State = Data.Action.ActionState;
    State.bAttacking = false;
    State.bContactOpen = false;
    State.ActionToken = INDEX_NONE;
    State.ContactToken = INDEX_NONE;
    State.bHasPreviousPose = false;
    State.HitActors.Reset();
    State.HitEntities.Reset();
}
void FBBBMeleeActionProcessor::Update(FBBBMeleeUpdateContext &Context)
{
    auto &State = Context.Data.Action.ActionState;
    const auto &Input = Context.Data.Action.ReadMeleeActionInputState();
    if (!Context.bCausal)
    {
        State.bContactOpen = false;
        return;
    }
    if (Input.bActionPermissionReceived)
    {
        State.bOwnerActionsAllowed = Input.bActionsAllowed;
    }
    if (!State.bOwnerActionsAllowed)
    {
        Stop(Context.Data);
        return;
    }

    for (int32 Token : Input.BeginActions)
    {
        if (State.bAttacking && State.ActionToken == INDEX_NONE)
        {
            State.ActionToken = Token;
        }
    }
    for (int32 Token : Input.BeginContacts)
    {
        if (State.bAttacking && State.ActionToken != INDEX_NONE && State.ContactToken == INDEX_NONE)
        {
            State.ContactToken = Token;
            State.bContactOpen = true;
            State.bHasPreviousPose = false;
            UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Window opened Attack=%d Token=%d"), State.AttackSequence, Token);
        }
    }
    for (int32 Token : Input.EndContacts)
    {
        if (Token == State.ContactToken)
        {
            State.bContactOpen = false;
            State.ContactToken = INDEX_NONE;
            State.bHasPreviousPose = false;
        }
    }
    for (int32 Token : Input.EndActions)
    {
        if (Token == State.ActionToken)
        {
            Stop(Context.Data);
            UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Attack ended Sequence=%d"), State.AttackSequence);
        }
    }
    if (!Input.bPrimary || State.bAttacking)
    {
        return;
    }
    const double Time = Context.World.GetTimeSeconds();
    if (Time - State.LastAttackTime < Context.Definition.AttackInterval)
    {
        return;
    }
    Stop(Context.Data);
    ++State.AttackSequence;
    State.LastAttackTime = Time;
    State.bAttacking = true;
    UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Attack started Equipment=%s Sequence=%d Time=%.3f"),
        *Context.Equipment.GetEquipmentId().ToString(), State.AttackSequence, Time);
}
