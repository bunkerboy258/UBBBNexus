
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentDefinition.h"

ABBBEquipmentActor::ABBBEquipmentActor()
{

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bReplicates = false;
    SetReplicateMovement(false);
}

void ABBBEquipmentActor::InitializeEquipment(
    const FBBBItemInstance &InItemInstance,
    FBBBCharacterExternalAPI &InCharacterAPI)
{

    ItemInstance = InItemInstance;

    CharacterAPI = &InCharacterAPI;

    if (!ensureMsgf(ItemInstance.IsValid(), TEXT("[UBBBI]Equipment actor received invalid item instance")))
    {
        return;
    }

    SetActorTickEnabled(true);
}

void ABBBEquipmentActor::Tick(float DeltaSeconds)
{

    Super::Tick(DeltaSeconds);
}

void ABBBEquipmentActor::Equip()
{

    const UBBBEquipmentDefinition *Definition = Cast<UBBBEquipmentDefinition>(ItemInstance.Definition);
    if (!ensureMsgf(Definition, TEXT("[UBBBI]Item definition is not UBBBEquipmentDefinition")))
    {
        return;
    }

    FBBBCharacterAnimationRequest Request;
    Request.Montage = Definition->EquipMontage;

    QueueMontage(Request);
}

void ABBBEquipmentActor::QueueMontage(const FBBBCharacterAnimationRequest &Request) const
{

    if (!Request.Montage)
    {
        return;
    }

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Character API is null when queueing equipment montage")))
    {
        return;
    }

    CharacterAPI->QueueMontage(Request);
}
