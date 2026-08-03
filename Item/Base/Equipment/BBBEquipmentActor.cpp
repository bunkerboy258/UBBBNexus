
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"

ABBBEquipmentActor::ABBBEquipmentActor()
{

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bReplicates = false;
    SetReplicateMovement(false);
}

void ABBBEquipmentActor::InitializeRuntimeEquipment(
    const FBBBItemInstance &InItemInstance,
    FBBBCharacterExternalAPI &InCharacterAPI)
{
    if (!ensureMsgf(InItemInstance.IsValid(), TEXT("[UBBBI]Equipment actor received invalid runtime item instance")))
    { return; }

    const UBBBEquipmentDefinition *Definition = Cast<UBBBEquipmentDefinition>(InItemInstance.Definition);
    if (!ensureMsgf(Definition, TEXT("[UBBBI]Runtime item definition is not an equipment definition")))
    { return; }

    ItemInstance = InItemInstance;
    InitializeCommon(*Definition, InCharacterAPI, EBBBEquipmentInstanceMode::Runtime);
}

void ABBBEquipmentActor::InitializeEquipmentMirror(
    const UBBBEquipmentDefinition &InDefinition,
    FBBBCharacterExternalAPI &InCharacterAPI)
{
    ItemInstance = FBBBItemInstance();
    InitializeCommon(InDefinition, InCharacterAPI, EBBBEquipmentInstanceMode::Mirror);
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

EBBBEquipmentInstanceMode ABBBEquipmentActor::GetInstanceMode() const
{
    return InstanceMode;
}

void ABBBEquipmentActor::InitializeCommon(
    const UBBBEquipmentDefinition &InDefinition,
    FBBBCharacterExternalAPI &InCharacterAPI,
    EBBBEquipmentInstanceMode InInstanceMode)
{
    EquipmentDefinition = &InDefinition;
    CharacterAPI = &InCharacterAPI;
    InstanceMode = InInstanceMode;

    SetActorTickEnabled(InstanceMode == EBBBEquipmentInstanceMode::Runtime);
}

void ABBBEquipmentActor::QueueMontage(const FBBBCharacterAnimationRequest &Request) const
{

    if (!Request.Montage)
    { return; }

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Character API is null when queueing equipment montage")))
    { return; }

    CharacterAPI->QueueMontage(Request);
}
