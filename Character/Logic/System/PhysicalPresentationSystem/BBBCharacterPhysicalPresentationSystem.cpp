#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/BBBCharacterPhysicalPresentationSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"

void FBBBCharacterPhysicalPresentationSystem::Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData,
                                                         USkeletalMeshComponent &InMesh,
                                                         UBBBCharacterHitReactionComponent &InReaction,
                                                         UPhysicalAnimationComponent &InPhysicalAnimation)
{
    Character = &InCharacter;
    Data = &InData;
    Mesh = &InMesh;
    Reaction = &InReaction;
    PhysicalAnimation = &InPhysicalAnimation;
}

void FBBBCharacterPhysicalPresentationSystem::Update() const
{
    if (!Character || !Data || !Mesh || !Reaction || !PhysicalAnimation)
    {
        return;
    }
    FBBBCharacterPhysicalPresentationUpdateContext Context{*Character, *Data, *Mesh, *Reaction, *PhysicalAnimation};
    KnockdownProcessor.Update(Context);
    FallProcessor.Update(Context);
    CandidateProcessor.Update(Context);
    PhaseProcessor.Update(Context);
    Processor.Update(Context);
}

void FBBBCharacterPhysicalPresentationSystem::CapturePhysics() const
{
    if (!Character || !Data || !Mesh || !Reaction || !PhysicalAnimation)
    {
        return;
    }
    FBBBCharacterPhysicalPresentationUpdateContext Context{*Character, *Data, *Mesh, *Reaction, *PhysicalAnimation};
    CaptureProcessor.Update(Context);
}
