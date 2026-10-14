#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePermissionLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterInteractionLifecycleProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    if (!Context.bHasSelectionResult)
    {
        return;
    }
    auto &Selection = Context.RuntimeData.Interaction.InteractionSelectionState;
    if (Context.PendingItemClass)
    {
        Context.DesiredItem = Create(Context.Character, Context.PendingItemClass);
        if (!Context.DesiredItem)
        {
            return;
        }
    }
    if (!IsValid(Context.DesiredItem))
    {
        Context.DesiredItem = nullptr;
    }
    if (Selection.ActiveMainHandInstance == Context.DesiredItem
        && (!Context.bIsMirror || Context.DesiredGeneration == 0 || Selection.ActiveGeneration == Context.DesiredGeneration))
    {
        return;
    }
    if (AActor *Previous = Selection.ActiveMainHandInstance)
    {
        Detach(Context.CharacterMesh, *Previous);
        if (Context.bIsMirror)
        {
            Previous->Destroy();
        }
    }
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveItemId = NAME_None;
    if (Context.bIsMirror)
    {
        Selection.ActiveGeneration = Context.DesiredGeneration;
    }
    if (!Context.bIsMirror)
    {
        ++Selection.ActiveGeneration;
    }
    AActor *Desired = Context.DesiredItem;
    if (!Desired)
    {
        return;
    }
    if (!ensureMsgf(Attach(Context.CharacterMesh, Context.RightHandSocketName, *Desired), TEXT("主手挂接失败 %s"), *GetNameSafe(Desired)))
    {
        if (Context.bIsMirror)
        {
            Desired->Destroy();
        }
        return;
    }
    Selection.ActiveMainHandInstance = Desired;
    if (const auto *Equipment = Cast<ABBBEquipment>(Desired))
    {
        Selection.ActiveItemId = Equipment->GetEquipmentId();
    }
    if (const auto *Consumable = Cast<ABBBConsumable>(Desired))
    {
        Selection.ActiveItemId = Consumable->GetDefinition()->ItemId;
    }
}

void FBBBCharacterInteractionLifecycleProcessor::Shutdown(FBBBCharacterInteractionUpdateContext &Context)
{
    auto &Selection = Context.RuntimeData.Interaction.InteractionSelectionState;
    if (AActor *Item = Selection.ActiveMainHandInstance)
    {
        Detach(Context.CharacterMesh, *Item);
        if (Context.bIsMirror)
        {
            Item->Destroy();
        }
    }
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveItemId = NAME_None;
    Selection.PendingItemId = NAME_None;
    Selection.bHasInteractionRequest = false;
}

AActor *FBBBCharacterInteractionLifecycleProcessor::Create(ABBBCharacter &Character, TSubclassOf<AActor> ItemClass)
{
    UWorld *World = Character.GetWorld();
    AActor *Item = World && ItemClass ? World->SpawnActorDeferred<AActor>(ItemClass, FTransform::Identity,
        &Character, &Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn) : nullptr;
    if (Item)
    {
        Item->SetActorHiddenInGame(true);
        UGameplayStatics::FinishSpawningActor(Item, FTransform::Identity);
        Item->SetActorTickEnabled(false);
    }
    const auto *Equipment = Cast<ABBBEquipment>(Item);
    const auto *Consumable = Cast<ABBBConsumable>(Item);
    if (!IsValid(Item) || !((Equipment && Equipment->IsInitialized()) || (Consumable && Consumable->IsInitialized())))
    {
        if (IsValid(Item))
        {
            Item->Destroy();
        }
        return nullptr;
    }
    return Item;
}

bool FBBBCharacterInteractionLifecycleProcessor::Attach(USkeletalMeshComponent &Mesh, FName Socket, AActor &Item)
{
    auto *Equipment = Cast<ABBBEquipment>(&Item);
    auto *Consumable = Cast<ABBBConsumable>(&Item);
    FTransform Offset;
    if (Consumable && Consumable->GetDefinition())
    {
        Socket = Consumable->GetDefinition()->HandSocketName;
    }
    const bool bValid = Equipment ? Equipment->GetEquipmentAnimationInstance() && Equipment->TryGetAttachmentOffset(Offset)
        : Consumable && Consumable->TryGetAttachmentOffset(Offset);
    if (!bValid || Item.GetOwner() != Mesh.GetOwner() || Socket.IsNone() || !Mesh.DoesSocketExist(Socket)
        || !Item.AttachToComponent(&Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket))
    {
        return false;
    }
    Item.SetActorRelativeTransform(Offset);
    Item.SetActorHiddenInGame(false);
    FBBBCharacterUpdatePipeline::RegisterItemTicks(Mesh, Item);
    Item.SetActorTickEnabled(true);
    return true;
}

void FBBBCharacterInteractionLifecycleProcessor::Detach(USkeletalMeshComponent &Mesh, AActor &Item)
{
    FBBBCharacterUpdatePipeline::UnregisterItemTicks(Mesh, Item);
    Item.SetActorHiddenInGame(true);
    Item.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    auto *Consumable = Cast<ABBBConsumable>(&Item);
    if (Consumable)
    {
        Consumable->SubmitInput(FBBBConsumablePermissionLocalControlPacket<>{{false}});
        Consumable->SubmitInput(FBBBConsumableFinishLocalControlPacket<>{});
    }
    // 装备卸下后保留一次更新 由自身关闭管线收束换弹与通知 再自行停止 Tick
    const auto *Equipment = Cast<ABBBEquipment>(&Item);
    Item.SetActorTickEnabled((Equipment && Equipment->IsInitialized()) || (Consumable && Consumable->IsUsing()));
}
