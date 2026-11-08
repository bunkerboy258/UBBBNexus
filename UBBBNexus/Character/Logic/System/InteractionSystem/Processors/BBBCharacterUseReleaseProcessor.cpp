#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterUseReleaseProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemConsumeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRecoveryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/BBBThrowableDefinition.h"
#include "GameFramework/Controller.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
void FBBBCharacterUseReleaseProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &Use = Context.RuntimeData.Interaction.InteractionUseState;
    auto &Inputs = Context.RuntimeData.Interaction.InteractionInputState;
    UWorld *World = Context.Character.GetWorld();
    const bool bActionsAllowed = Context.Character.IsItemUsable();
    const bool bHasNotification = Inputs.ReleaseTokens.Contains(Use.UseSerial);
    if (!Context.bIsMirror && Use.bBusy && bActionsAllowed && bHasNotification &&
        !Use.Fact.bCommitted && Use.Definition && World)
    {
        const auto &Inventory = Context.RuntimeData.Item.ReadCharacterItemInventoryState();
        const auto *Item = Inventory.Slots.FindByPredicate(
            [&Use](const auto &Value)
            {
                return Value.InstanceId == Use.Instance && Value.Quantity > 0;
            });
        bool bCanCommit = Item != nullptr;
        if (Use.Definition->bThrowable && bCanCommit)
        {
            const auto *Definition = Use.Definition->GetReleaseDefinition();
            bCanCommit = Definition && Definition->IsValid();
            if (bCanCommit)
            {
                const auto *Controller = Context.Character.GetController();
                FRotator Rotation =
                    Controller ? Controller->GetControlRotation() : Context.Character.GetActorRotation();
                Rotation.Pitch = FMath::Clamp(FRotator::NormalizeAxis(Rotation.Pitch) +
                                                  Use.Definition->GetReleasePitch(),
                                              -75.0f, 85.0f);
                const FTransform Hand = Context.CharacterMesh.GetSocketTransform(Use.Definition->HandSocketName);
                FVector Position = (Use.Definition->SpawnOffset * Hand).GetLocation();
                FVector Start = Context.Character.GetActorLocation();
                Start.Z = Position.Z;
                FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBConsumableRelease), false, &Context.Character);
                Query.AddIgnoredActor(Context.Character.GetHeldItem());
                FHitResult Hit;
                if (World->SweepSingleByProfile(Hit, Start, Position, FQuat::Identity,
                                                Definition->Physics.CollisionProfile,
                                                FCollisionShape::MakeSphere(Definition->Physics.Radius), Query))
                {
                    bCanCommit = !Hit.bStartPenetrating;
                    Position = Hit.Location + Hit.Normal;
                }
                Use.Fact.ReleaseTransform = FTransform(Rotation, Position);
                Use.Fact.InheritedVelocity = Context.Character.GetVelocity();
            }
        }
        if (!Use.Definition->bThrowable && bCanCommit)
        {
            bCanCommit = Context.Character.GetHealth() < Context.Character.GetCharacterConfig().MaximumHealth;
            if (bCanCommit)
            {
                Context.Character.SubmitInput(FBBBCharacterRecoveryLocalControlPacket{
                    {Context.Character.GetCharacterConfig().MaximumHealth * Use.Definition->GetRecoveryFraction()}});
            }
        }
        if (bCanCommit)
        {
            Use.Fact.bCommitted = true;
            ++Use.Fact.Revision;
            Context.Character.SubmitInput(FBBBCharacterItemConsumeLocalControlPacket{{Use.Instance}, {1}});
        }
    }
    Inputs.ReleaseRecipients.Reset();
    Inputs.ReleaseGenerations.Reset();
    Inputs.ReleaseTokens.Reset();

    auto Releases = MoveTemp(Inputs.ReleaseResults);
    Inputs.ReleaseResults.Reset();
    if (!Context.bIsMirror && Use.Fact.bCommitted && !Use.Fact.bQuickMelee)
    {
        Releases.Add(Use.Fact);
    }
    if (!World || !Context.Character.HasNetworkAuthority())
    {
        return;
    }
    Releases.Sort(
        [](const auto &Left, const auto &Right)
        {
            return Left.Serial < Right.Serial;
        });
    const auto *Catalog = Context.Character.GetCharacterConfig().Item.ConsumableCatalog.Get();
    for (const auto &Result : Releases)
    {
        auto *ItemDefinition = Catalog ? Catalog->FindItem(Result.ItemId) : nullptr;
        if (!ItemDefinition || !ItemDefinition->bThrowable || Result.Serial <= Use.SpawnedSerial)
        {
            continue;
        }
        auto *Definition = ItemDefinition->GetReleaseDefinition();
        if (!Definition || !Definition->IsValid())
        {
            continue;
        }
        auto *Throwable = World->SpawnActorDeferred<ABBBThrowable>(
            ABBBThrowable::StaticClass(), Result.ReleaseTransform, Context.Character.GetController(),
            &Context.Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        if (Throwable)
        {
            FBBBThrowableLaunchLocalControlPacket Packet;
            Packet.Definition = Definition;
            Packet.Transform = Result.ReleaseTransform;
            Packet.InheritedVelocity = Result.InheritedVelocity;
            Throwable->SubmitInput(MoveTemp(Packet));
            Throwable->FinishSpawning(Result.ReleaseTransform);
            Use.SpawnedSerial = Result.Serial;
        }
    }
}
