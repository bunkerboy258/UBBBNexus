#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"
#include "Components/SceneComponent.h"

UBBBEquipmentInstance *UBBBEquipmentInstance::Create(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition)
{
    if (!ensureMsgf(InDefinition.EquipDomin.IsValid(), TEXT("[UBBBE]Equipment definition has no equip domin")))
    {
        return nullptr;
    }

    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment instance creation failed")))
    {
        return nullptr;
    }

    // 分配唯一标识并绑定静态配置
    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->RuntimeData = NewObject<UBBBEquipmentRuntimeData>(Instance);

    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBE]Equipment runtime data creation failed")))
    {
        return nullptr;
    }

    Instance->RuntimeData->Initialize(InDefinition);

    Instance->EquipmentSystem = NewObject<UBBBEquipmentSystem>(Instance);
    if (!ensureMsgf(Instance->EquipmentSystem, TEXT("[UBBBE]Equipment system creation failed")))
    {
        return nullptr;
    }

    if (!Instance->EquipmentSystem->Initialize(*Instance, InDefinition, *Instance->RuntimeData))
    {
        return nullptr;
    }

    return Instance;
}

//------------------------------------------------------------------------------

const FGuid &UBBBEquipmentInstance::GetInstanceId() const
{
    return InstanceId;
}

//------------------------------------------------------------------------------

UBBBEquipmentDefinition *UBBBEquipmentInstance::GetDefinition() const
{
    return Definition;
}

//------------------------------------------------------------------------------

UBBBEquipmentSystem *UBBBEquipmentInstance::GetEquipmentSystem() const
{
    return EquipmentSystem;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::TryGetSocketTransforms(
    const FName SocketName,
    FTransform &OutSocketComponentSpace,
    FTransform &OutEquipmentWorld) const
{
    OutSocketComponentSpace = FTransform::Identity;
    OutEquipmentWorld = FTransform::Identity;

    if (!ensureMsgf(PresentationActor, TEXT("[UBBBE]Equipment presentation actor is null during socket query")))
    {
        return false;
    }

    USceneComponent *EquipmentComponent = PresentationActor->GetEquipmentAttachmentComponent();
    if (!ensureMsgf(EquipmentComponent, TEXT("[UBBBE]Equipment attachment component is null during socket query")))
    {
        return false;
    }

    if (!ensureMsgf(
        EquipmentComponent->DoesSocketExist(SocketName),
        TEXT("[UBBBE]Equipment socket '%s' is missing"),
        *SocketName.ToString()))
    {
        return false;
    }

    OutSocketComponentSpace = EquipmentComponent->GetSocketTransform(
        SocketName,
        RTS_Component);
    OutEquipmentWorld = EquipmentComponent->GetComponentTransform();
    return true;
}
