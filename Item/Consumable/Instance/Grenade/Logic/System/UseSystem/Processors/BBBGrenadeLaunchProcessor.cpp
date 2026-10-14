#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeLaunchProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemConsumeLocalControlPacket.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

void FBBBGrenadeLaunchProcessor::Update(FBBBGrenadeUseUpdateContext& Context) const
{
    auto& State = Context.Data.Use.UseState;
    const auto& Inputs = Context.Data.Use.ReadGrenadeUseInputState();
    const bool bCausalRelease = !Context.bIsMirror && Inputs.bRelease && State.bUsing
        && !State.bReleased && State.bAllowed && Context.Character.GetHeldItemQuantity() > 0;
    const bool bHostRelease = Context.Character.HasNetworkAuthority() && Context.bIsMirror
        && State.bReleased && State.ReleaseSerial == State.UseSerial
        && State.ReleaseSerial > State.SpawnedReleaseSerial;
    if (!bCausalRelease && !bHostRelease)
    {
        return;
    }
    auto* Definition = Context.Definition.Throwable.Get();
    if (!Definition || !Definition->IsValid())
    {
        return;
    }
    if (bCausalRelease)
    {
        const auto* Controller = Context.Character.GetController();
        FRotator Rotation = Controller ? Controller->GetControlRotation() : Context.Character.GetActorRotation();
        Rotation.Pitch = FMath::Clamp(FRotator::NormalizeAxis(Rotation.Pitch) + Context.Definition.PitchOffset, -75.0f, 85.0f);
        State.LaunchTransform = FTransform(Rotation, Context.Actor.GetActorLocation(), FVector::OneVector);
        State.InheritedVelocity = Context.Character.GetVelocity();
    }
    if (Context.Character.HasNetworkAuthority())
    {
        ABBBThrowable* Throwable = Context.World.SpawnActorDeferred<ABBBThrowable>(ABBBThrowable::StaticClass(),
            State.LaunchTransform, Context.Character.GetController(), &Context.Character,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        if (!Throwable)
        {
            UE_LOG(LogTemp, Error, TEXT("[BBBGrenade] Throwable spawn rejected"));
            return;
        }
        FBBBThrowableLaunchLocalControlPacket Packet;
        Packet.Definition = Definition;
        Packet.Transform = State.LaunchTransform;
        Packet.InheritedVelocity = State.InheritedVelocity;
        if (!Throwable->SubmitInput(MoveTemp(Packet)))
        {
            Throwable->Destroy();
            return;
        }
        Throwable->FinishSpawning(State.LaunchTransform);
        State.SpawnedReleaseSerial = bCausalRelease ? State.UseSerial : State.ReleaseSerial;
    }
    if (bCausalRelease)
    {
        State.ReleaseSerial = State.UseSerial;
        State.bReleased = true;
        Context.Character.SubmitInput(FBBBCharacterItemConsumeLocalControlPacket{{Context.Character.GetHeldItemIdentity()}, {1}});
    }
    UE_LOG(LogTemp, Log, TEXT("[BBBGrenade] Released Serial=%d Causal=%d Authority=%d Position=%s"),
        State.ReleaseSerial, bCausalRelease, Context.Character.HasNetworkAuthority(), *State.LaunchTransform.GetLocation().ToString());
}
