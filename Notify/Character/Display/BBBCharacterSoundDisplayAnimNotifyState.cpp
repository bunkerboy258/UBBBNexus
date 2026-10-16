#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterSoundDisplayAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"

void UBBBCharacterSoundDisplayAnimNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp,
                                                           UAnimSequenceBase *Animation, float TotalDuration,
                                                           const FAnimNotifyEventReference &EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    if (!MeshComp || !Cast<ABBBCharacter>(MeshComp->GetOwner()) || !Sound || !MeshComp->GetWorld() ||
        MeshComp->GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }
    if (auto *Previous = Playing.FindRef(MeshComp).Get())
    {
        Previous->FadeOut(0.03f, 0.0f);
    }
    auto *Audio = UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, SocketName, FVector::ZeroVector,
                                                       EAttachLocation::KeepRelativeOffset, true, Volume, Pitch, 0.0f,
                                                       Attenuation);
    Playing.Add(MeshComp, Audio);
}

void UBBBCharacterSoundDisplayAnimNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
                                                         const FAnimNotifyEventReference &EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);
    if (bStopOnEnd)
    {
        if (auto *Audio = Playing.FindRef(MeshComp).Get())
        {
            Audio->FadeOut(0.04f, 0.0f);
        }
    }
    Playing.Remove(MeshComp);
}
