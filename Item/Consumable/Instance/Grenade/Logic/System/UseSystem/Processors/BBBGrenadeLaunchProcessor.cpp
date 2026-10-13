#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeLaunchProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemConsumeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "NiagaraDataChannelAsset.h"
#include "NiagaraSystem.h"

void FBBBGrenadeLaunchProcessor::Update(FBBBGrenadeUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    const auto &Inputs = Context.Data.Use.ReadGrenadeUseInputState();
    const bool bCausalRelease = !Context.bIsMirror && Inputs.bRelease && State.bUsing
        && !State.bReleased && State.bAllowed && Context.Character.GetHeldItemQuantity() > 0;
    const bool bMirrorRelease = Context.bIsMirror && State.ReleaseSerial > State.SpawnedReleaseSerial;
    if (!bCausalRelease && !bMirrorRelease)
    {
        return;
    }
    const UBBBProjectileDefinition *Definition = Context.Definition.Projectile;
    UBBBMassSubsystem *Mass = Context.World.GetSubsystem<UBBBMassSubsystem>();
    if (!Definition || !Definition->IsValid() || !Mass)
    {
        return;
    }
    FTransform LaunchTransform = State.LaunchTransform;
    if (bCausalRelease)
    {
        const auto *Controller = Context.Character.GetController();
        FRotator Rotation = Controller ? Controller->GetControlRotation() : Context.Character.GetActorRotation();
        Rotation.Pitch = FMath::Clamp(FRotator::NormalizeAxis(Rotation.Pitch) + Context.Definition.PitchOffset, -75.0f, 85.0f);
        LaunchTransform = FTransform(Rotation, Context.Actor.GetActorLocation(), FVector::OneVector);
    }

    FBBBProjectileSpawnLocalControlPacket Packet;
    Packet.LaunchTransform = LaunchTransform;
    Packet.GravityScale = Definition->GravityScale;
    Packet.ExplosionRadiusCm = Definition->ExplosionRadiusCm;
    Packet.FuseSeconds = Definition->FuseSeconds;
    Packet.bDetonateOnImpact = Definition->bDetonateOnImpact;
    Packet.bBounceOnImpact = Definition->bBounceOnImpact;
    Packet.BounceRestitution = Definition->BounceRestitution;
    Packet.bDamageInstigator = Definition->bDamageInstigator;
    Packet.Mesh = Definition->Mesh.Get();
    Packet.MeshRelativeTransform = Definition->MeshRelativeTransform;
    Packet.Speed = Definition->InitialSpeedCmPerSecond;
    Packet.Lifetime = Definition->MaximumLifetimeSeconds;
    Packet.Damage = Definition->BaseDamage;
    Packet.DurableDamage = Definition->DurableDamage;
    Packet.StaggerValue = Definition->StaggerValue;
    Packet.Radius = Definition->CollisionRadiusCm;
    Packet.CollisionChannel = Definition->CollisionChannel;
    Packet.Source = &Context.Character;
    Packet.Pawn = &Context.Character;
    Packet.Controller = Context.Character.GetController();
    Packet.Channel = Definition->PresentationChannel.Get();
    Packet.System = Definition->PresentationSystem.Get();
    Packet.ImpactChannel = Definition->ImpactChannel.Get();
    Packet.TracerLengthCm = Definition->TracerLengthCm;
    Packet.TracerWidthCm = Definition->TracerWidthCm;
    Packet.TracerColor = Definition->TracerColor;
    Packet.bCanCauseDamage = !Context.bIsMirror;
    if (!Packet.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[BBBGrenade] Invalid release configuration"));
        return;
    }
    const FMassEntityHandle Entity = Mass->CreateEntity(*Definition->EntityConfig);
    if (!Entity.IsSet() || !Mass->SubmitInput(Entity, Packet))
    {
        if (Entity.IsSet())
        {
            Mass->DestroyEntity(Entity);
        }
        UE_LOG(LogTemp, Error, TEXT("[BBBGrenade] Projectile spawn rejected"));
        return;
    }
    if (bCausalRelease)
    {
        State.LaunchTransform = LaunchTransform;
        State.ReleaseSerial = State.UseSerial;
        State.bReleased = true;
        Context.Character.SubmitInput(FBBBCharacterItemConsumeLocalControlPacket{{Context.Character.GetHeldItemIdentity()}, {1}});
    }
    State.SpawnedReleaseSerial = State.ReleaseSerial;
    UE_LOG(LogTemp, Log, TEXT("[BBBGrenade] Released Serial=%d Causal=%d Position=%s"),
        State.ReleaseSerial, !Context.bIsMirror, *LaunchTransform.GetLocation().ToString());
}
