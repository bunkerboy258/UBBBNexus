#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Context/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

ABBBEquipmentActor::ABBBEquipmentActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bReplicates = false;
    SetReplicateMovement(false);
}

void ABBBEquipmentActor::Initialize(
    UBBBEquipmentInstance &InEquipmentInstance,
    FBBBCharacterExternalAPI &InCharacterAPI)
{
    const UBBBEquipmentDefinition *Definition = InEquipmentInstance.GetEquipmentDefinition();
    if (!ensureMsgf(Definition, TEXT("[UBBBI]Equipment instance definition is invalid")))
    { return; }

    EquipmentInstance = &InEquipmentInstance;
    EquipmentDefinition = Definition;
    CharacterAPI = &InCharacterAPI;
}

void ABBBEquipmentActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ABBBEquipmentActor::Equip()
{
    if (!ensureMsgf(EquipmentDefinition, TEXT("[UBBBI]Equipment definition is null during equip")))
    { return; }

    FBBBCharacterAnimationRequest Request;
    Request.Montage = EquipmentDefinition->EquipMontage;

    QueueMontage(Request);
}

bool ABBBEquipmentActor::Fire()
{
    return false;
}

bool ABBBEquipmentActor::Reload()
{
    return false;
}

void ABBBEquipmentActor::PresentFire()
{
}

void ABBBEquipmentActor::PresentReload()
{
}

const UBBBEquipmentDefinition *ABBBEquipmentActor::GetEquipmentDefinition() const
{
    return EquipmentDefinition;
}

void ABBBEquipmentActor::QueueMontage(const FBBBCharacterAnimationRequest &Request) const
{
    if (!Request.Montage)
    { return; }

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Character API is null when queueing equipment montage")))
    { return; }

    CharacterAPI->QueueMontage(Request);
}
