#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterExternalAPI::Initialize(ABBBCharacterInstance &Character, FBBBAnimationRuntimeData &Animation,
    FBBBInputRuntimeData &Input, FBBBCharacterEquipmentEvents &Events,
    FBBBCharacterEquipmentState &EquipmentState)
{
    Owner = &Character;
    AnimationData = &Animation;
    InputData = &Input;
    EquipmentEvents = &Events;
    CharacterEquipmentState = &EquipmentState;
}

bool FBBBCharacterExternalAPI::SubmitEquipmentMontage(UAnimMontage *Montage, const float PlayRate,
    const int32 Sequence, const bool bReload)
{
    if (!ensureMsgf(IsInGameThread() && AnimationData && Montage
        && FMath::IsFinite(PlayRate) && PlayRate > 0.0f, TEXT("[UBBBC]Invalid montage contribution")))
    {
        return false;
    }

    FBBBCharacterMontagePacket Packet;
    Packet.Montage = Montage;
    Packet.PlayRate = PlayRate;
    Packet.Sequence = Sequence;
    Packet.bReload = bReload;
    AnimationData->MontageQueue.Add(MoveTemp(Packet));
    return true;
}

void FBBBCharacterExternalAPI::SubmitReloadInput(const int32 Sequence, const EBBBCharacterReloadAnimationPhase Phase)
{
    if (!ensureMsgf(IsInGameThread() && Owner && InputData, TEXT("[UBBBC]Reload input dependencies are invalid")))
    {
        return;
    }

    if (!Owner->IsLocallyControlled())
    {
        return;
    }

    if (!ensureMsgf(Sequence > 0, TEXT("[UBBBC]Reload animation input has no action sequence")))
    {
        return;
    }

    FBBBCharacterReloadAnimationInput Input;
    Input.Sequence = Sequence;
    Input.Phase = Phase;
    InputData->PendingReloadInputs.Add(Input);
}

void FBBBCharacterExternalAPI::SubmitReloadStartNotify(const int32 Sequence)
{
    SubmitReloadInput(Sequence, EBBBCharacterReloadAnimationPhase::Start);
}

void FBBBCharacterExternalAPI::SubmitReloadEndNotify(const int32 Sequence, const EBBBCharacterReloadEndReason EndReason)
{
    if (EndReason == EBBBCharacterReloadEndReason::PlaybackFailed)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload montage failed or missed its end notify Sequence=%d"), Sequence);
    }
    SubmitReloadInput(Sequence, EndReason == EBBBCharacterReloadEndReason::Loaded
        ? EBBBCharacterReloadAnimationPhase::End : EBBBCharacterReloadAnimationPhase::Interrupted);
}

void FBBBCharacterExternalAPI::PublishEquipmentEvent(const FBBBEquipmentActionEvent &Event)
{
    if (ensureMsgf(IsInGameThread() && EquipmentEvents && CharacterEquipmentState,
        TEXT("[UBBBC]Equipment result sink is unavailable")))
    {
        EquipmentEvents->AddAction(Event);
        ABBBEquipmentInstance *ActiveEquipment = CharacterEquipmentState->GetActiveMainHandInstance();
        if (ActiveEquipment && Event.EquipmentId == ActiveEquipment->GetEquipmentId())
        {
            if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadStarted)
            {
                CharacterEquipmentState->ReloadSequence = Event.Sequence;
            }
            if ((Event.Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
                || Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
                && Event.Sequence == CharacterEquipmentState->ReloadSequence)
            {
                CharacterEquipmentState->ReloadSequence = INDEX_NONE;
            }
        }

        if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled && AnimationData)
        {
            AnimationData->MontageQueue.RemoveAll([&Event](const FBBBCharacterMontagePacket &Packet)
            {
                return Packet.bReload && Packet.Sequence == Event.Sequence;
            });
            AnimationData->CancelledReloadSequences.AddUnique(Event.Sequence);
        }
    }
}

void FBBBCharacterExternalAPI::SubmitEquipmentRecoil(const FVector2D Impulse, const float RecoverySpeed)
{
    if (!ensureMsgf(IsInGameThread() && EquipmentEvents && !Impulse.ContainsNaN()
        && FMath::IsFinite(RecoverySpeed) && RecoverySpeed > 0.0f, TEXT("[UBBBC]Invalid recoil contribution")))
    {
        return;
    }

    FBBBEquipmentRecoilEvent Event;
    Event.Impulse = Impulse;
    Event.RecoverySpeed = RecoverySpeed;
    EquipmentEvents->AddRecoil(Event);
}
