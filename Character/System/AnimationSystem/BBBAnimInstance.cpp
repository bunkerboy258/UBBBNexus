
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"

void UBBBAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    RefreshCachedReferences();
}

void UBBBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!AnimationState || !AimState || !EquipmentCommands)
    {

        RefreshCachedReferences();
    }
}

void UBBBAnimInstance::RefreshCachedReferences()
{

    ABBBCharacter *Character = Cast<ABBBCharacter>(TryGetPawnOwner());
    AnimationState = Character
        ? &Character->GetAnimationState()
        : nullptr;
    AimState = Character
        ? &Character->GetAimState()
        : nullptr;
    EquipmentCommands = Character
        ? &Character->RuntimeData.Equipment.Commands
        : nullptr;
}

const FBBBCharacterAnimationState &UBBBAnimInstance::GetAnimationState() const
{
    if (AnimationState)
    {
        return *AnimationState;
    }
    static const FBBBCharacterAnimationState EmptyState;
    return EmptyState;
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::SubmitRemoveMagazine()
{
    if (EquipmentCommands)
    {
        EquipmentCommands->SubmitRemoveMagazine();
    }
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::SubmitSpawnMagazine()
{
    if (EquipmentCommands)
    {
        EquipmentCommands->SubmitSpawnMagazine();
    }
}
