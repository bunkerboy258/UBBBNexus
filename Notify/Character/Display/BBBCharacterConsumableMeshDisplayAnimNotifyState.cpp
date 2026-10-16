#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterConsumableMeshDisplayAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
void UBBBCharacterConsumableMeshDisplayAnimNotifyState::NotifyBegin(USkeletalMeshComponent *Mesh,
                                                                    UAnimSequenceBase *Animation, float Duration,
                                                                    const FAnimNotifyEventReference &Reference)
{
    Super::NotifyBegin(Mesh, Animation, Duration, Reference);
    auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (!Character)
    {
        return;
    }
    const auto &Use = Character->RuntimeData.Interaction.ReadInteractionUseState();
    const auto *Definition = Use.Definition.Get();
    if (!Use.Fact.bActive || !Definition || !Definition->Mesh || !Mesh->DoesSocketExist(Definition->HandSocketName))
    {
        return;
    }
    if (auto *Previous = Displays.FindRef(Mesh).Get())
    {
        Previous->DestroyComponent();
    }
    auto *Display = NewObject<UStaticMeshComponent>(Character);
    Display->SetStaticMesh(Definition->Mesh);
    Display->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Display->SetGenerateOverlapEvents(false);
    Display->SetCanEverAffectNavigation(false);
    Display->SetupAttachment(Mesh, Definition->HandSocketName);
    Display->SetRelativeTransform(Definition->SpawnOffset);
    Display->RegisterComponent();
    Displays.Add(Mesh, Display);
}
void UBBBCharacterConsumableMeshDisplayAnimNotifyState::NotifyTick(USkeletalMeshComponent *Mesh,
                                                                   UAnimSequenceBase *Animation, float DeltaTime,
                                                                   const FAnimNotifyEventReference &Reference)
{
    Super::NotifyTick(Mesh, Animation, DeltaTime, Reference);
    const auto *Character = Mesh ? Cast<ABBBCharacter>(Mesh->GetOwner()) : nullptr;
    if (!Character || !Character->RuntimeData.Interaction.ReadInteractionUseState().Fact.bActive)
    {
        if (auto *Display = Displays.FindRef(Mesh).Get())
        {
            Display->DestroyComponent();
        }
        Displays.Remove(Mesh);
    }
}
void UBBBCharacterConsumableMeshDisplayAnimNotifyState::NotifyEnd(USkeletalMeshComponent *Mesh,
                                                                  UAnimSequenceBase *Animation,
                                                                  const FAnimNotifyEventReference &Reference)
{
    Super::NotifyEnd(Mesh, Animation, Reference);
    if (auto *Display = Displays.FindRef(Mesh).Get())
    {
        Display->DestroyComponent();
    }
    Displays.Remove(Mesh);
}
