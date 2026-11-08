#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AnimationSystem/Processors/BBBMaodieTestAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Animation/BBBMaodieTestAnimInstance.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInstanceDynamic.h"
void FBBBMaodieTestAnimationProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
{
    auto &State = Actor.RuntimeData.Animation.State;
    const auto &Snapshot = Actor.RuntimeData.Behavior.ReadMaodieTestBehaviorState().Snapshot;
    const auto &Config = Actor.GetTestConfig();
    auto *Instance = Cast<UBBBMaodieTestAnimInstance>(Actor.GetMesh()->GetAnimInstance());
    if (State.LastPhase != Snapshot.Phase)
    {
        const bool bWasMoving = State.LastPhase == 1;
        const bool bMoving = Snapshot.Phase == 1;
        if (Instance && (State.LastPhase == 255 || bWasMoving != bMoving))
        {
            Instance->SetAnimationAsset(bMoving ? Config.WalkAnimation : Config.IdleAnimation, true);
            Instance->PlayAnim(true);
        }
        State.LastPhase = Snapshot.Phase;
    }
    FRotator DesiredHeadRotation = FRotator::ZeroRotator;
    if (Snapshot.bHasLookTarget)
    {
        const FVector Direction = Actor.GetActorQuat().UnrotateVector(
            Snapshot.LookTarget - Actor.GetMesh()->GetSocketLocation(TEXT("head")));
        const FRotator LookRotation = Direction.Rotation();
        DesiredHeadRotation.Yaw = FMath::Clamp(FMath::UnwindDegrees(LookRotation.Yaw), -Config.HeadYawLimit, Config.HeadYawLimit);
        DesiredHeadRotation.Pitch = FMath::Clamp(FMath::UnwindDegrees(LookRotation.Pitch), -Config.HeadPitchLimit, Config.HeadPitchLimit);
    }
    State.HeadRotation = FMath::RInterpTo(State.HeadRotation, DesiredHeadRotation, DeltaSeconds, Config.HeadTurnSpeed);
    if (Instance)
    {
        Instance->HeadRotation = State.HeadRotation;
    }
    if (!State.ExpressionMaterial)
    {
        State.ExpressionMaterial = Actor.GetMesh()->CreateDynamicMaterialInstance(0);
    }
    if (State.ExpressionMaterial)
    {
        State.ExpressionMaterial->SetScalarParameterValue(TEXT("Hiss"), Snapshot.Phase == 3 ? 1.0f : 0.0f);
    }
    if (Snapshot.Phase != 3 && IsValid(State.HissAudio))
    {
        State.HissAudio->Stop();
        State.HissAudio = nullptr;
    }
    if (State.LastHissSerial != Snapshot.HissSerial)
    {
        State.LastHissSerial = Snapshot.HissSerial;
        if (Snapshot.Phase == 3 && Actor.GetNetMode() != NM_DedicatedServer)
        {
            if (IsValid(State.HissAudio))
            {
                State.HissAudio->Stop();
            }
            State.HissAudio = UGameplayStatics::SpawnSoundAttached(Config.HissSound, Actor.GetMesh(), TEXT("head"),
                FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
        }
    }
}
