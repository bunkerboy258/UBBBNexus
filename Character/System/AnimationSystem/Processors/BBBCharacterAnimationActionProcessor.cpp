#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance,
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    const FBBBCharacterAnimationConfig &AnimationConfig) const
{
    const TArray<FBBBEquipmentActionEvent> &ActionEvents = EquipmentEvents.GetActionEvents();
    for (const FBBBEquipmentActionEvent &Event : ActionEvents)
    {
        UAnimMontage *Montage = nullptr;

        switch (Event.ActionType)
        {
            case EBBBCharacterActionType::Equip:
                Montage = AnimationConfig.Weapon.EquipMontage;
                break;

            case EBBBCharacterActionType::Reload:
                Montage = AnimationConfig.Weapon.ReloadMontage;
                break;

            case EBBBCharacterActionType::Fire:
                Montage = AnimationConfig.Weapon.FireMontage;
                break;

            default:
                break;
        }

        if (!Montage)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("[UBBBC]No montage configured for equipment action %d"),
                static_cast<int32>(Event.ActionType));
            continue;
        }

        float PlayRate = 1.0f;
        if (Event.DurationSeconds > 0.0f)
        {
            PlayRate = FMath::Max(
                Montage->GetPlayLength() / Event.DurationSeconds,
                0.01f);
        }

        AnimInstance.PublishEquipmentAction(
            Event.ActionType,
            Event.Sequence,
            Event.DurationSeconds,
            *Montage,
            PlayRate);
    }
}
