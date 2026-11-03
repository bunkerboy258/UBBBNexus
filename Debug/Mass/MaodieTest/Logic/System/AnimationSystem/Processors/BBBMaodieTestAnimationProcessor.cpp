#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AnimationSystem/Processors/BBBMaodieTestAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
void FBBBMaodieTestAnimationProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
{
    auto &State = Actor.RuntimeData.Animation.State;
    const auto &Snapshot = Actor.RuntimeData.Behavior.ReadMaodieTestBehaviorState().Snapshot;
    const auto &Config = Actor.GetTestConfig();
    if (State.LastPhase != Snapshot.Phase)
    {
        const bool bWasMoving = State.LastPhase == 1;
        const bool bMoving = Snapshot.Phase == 1;
        if (State.LastPhase == 255 || bWasMoving != bMoving)
        {
            Actor.GetMesh()->PlayAnimation(bMoving ? Config.WalkAnimation : Config.IdleAnimation, true);
        }
        State.LastPhase = Snapshot.Phase;
    }
    Actor.GetMesh()->SetMorphTarget(TEXT("Hiss"), Snapshot.Phase == 3 ? 1.0f : 0.0f);
    if (State.LastHissSerial != Snapshot.HissSerial)
    {
        State.LastHissSerial = Snapshot.HissSerial;
        if (Snapshot.Phase == 3 && Actor.GetNetMode() != NM_DedicatedServer)
        {
            UGameplayStatics::PlaySoundAtLocation(&Actor, Config.HissSound, Actor.GetActorLocation(), 1.0f, 1.0f);
        }
    }
}
