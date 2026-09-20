#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterUpdatePipeline::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;
    LateUpdateTick.SetTickFunctionEnable(true);
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::RegisterTickFunctions(
    ABBBCharacter &InCharacter,
    const bool bRegister)
{
    Character = &InCharacter;

    UCharacterMovementComponent *Movement = InCharacter.GetCharacterMovement();
    USkeletalMeshComponent *CharacterMesh = InCharacter.GetMesh();
    if (!Movement || !CharacterMesh)
    {
        return;
    }

    if (bRegister)
    {
        LateUpdateTick.bCanEverTick = true;
        LateUpdateTick.bStartWithTickEnabled = false;
        LateUpdateTick.TickGroup = TG_PrePhysics;
        LateUpdateTick.Target = &InCharacter;
        LateUpdateTick.Pipeline = this;
        LateUpdateTick.SetTickFunctionEnable(InCharacter.HasActorBegunPlay());
        LateUpdateTick.AddPrerequisite(Movement, Movement->PrimaryComponentTick);
        LateUpdateTick.RegisterTickFunction(InCharacter.GetLevel());
        CharacterMesh->PrimaryComponentTick.AddPrerequisite(&InCharacter, LateUpdateTick);
        return;
    }

    CharacterMesh->PrimaryComponentTick.RemovePrerequisite(&InCharacter, LateUpdateTick);
    LateUpdateTick.RemovePrerequisite(Movement, Movement->PrimaryComponentTick);
    LateUpdateTick.UnRegisterTickFunction();
    LateUpdateTick.Pipeline = nullptr;
    LateUpdateTick.Target = nullptr;
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::Update(const float DeltaSeconds) const
{
    if (!Character)
    {
        return;
    }

    const UWorld *World = Character->GetWorld();
    if (!World)
    {
        return;
    }

    Character->RuntimeData.WorldData.Update(DeltaSeconds, World->GetTimeSeconds());

    const bool bAuthority = Character->HasAuthority();
    const bool bLocallyControlled = Character->IsLocallyControlled();

    Character->ParseSystem.Update();
    Character->EquipmentController.Update();

    if (bAuthority || bLocallyControlled)
    {
        Character->AimController.Update();
        Character->LocomotionController.Update();
    }

    Character->NetworkSystem.Update();
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::LateUpdate() const
{
    if (!Character)
    {
        return;
    }

    Character->AnimationSystem.Update();
    Character->RuntimeData.Clean();
}
