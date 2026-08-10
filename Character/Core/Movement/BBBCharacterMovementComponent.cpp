#include "BBBWork/UBBBNexus/Character/Core/Movement/BBBCharacterMovementComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void UBBBCharacterMovementComponent::OnMovementUpdated(
    float DeltaSeconds,
    const FVector &OldLocation,
    const FVector &OldVelocity)
{
    Super::OnMovementUpdated(
        DeltaSeconds,
        OldLocation,
        OldVelocity);

    ABBBCharacter *Character = Cast<ABBBCharacter>(CharacterOwner);

    if (!ensureMsgf(Character, TEXT("[UBBBC]Post-movement update failed because character owner is invalid")))
    {
        return;
    }

    Character->UpdateAfterCharacterMovement();
}
