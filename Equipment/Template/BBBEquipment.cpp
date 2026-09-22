#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"

#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Definition/BBBEquipmentDefinition.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

ABBBEquipment::ABBBEquipment()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;
    SetActorEnableCollision(false);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentSkeletalMesh"));
    EquipmentSkeletalMesh->SetupAttachment(EquipmentRoot);
    EquipmentSkeletalMesh->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->PrimaryComponentTick.EndTickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentSkeletalMesh->SetGenerateOverlapEvents(false);
    PrimaryActorTick.AddPrerequisite(EquipmentSkeletalMesh, EquipmentSkeletalMesh->PrimaryComponentTick);
}

const FGuid &ABBBEquipment::GetInstanceId() const
{
    return InstanceId;
}

FName ABBBEquipment::GetEquipmentId() const
{
    return Definition ? Definition->EquipmentId : NAME_None;
}

TSubclassOf<UAnimInstance> ABBBEquipment::GetCharacterAnimationLayerClass() const
{
    return Definition ? Definition->CharacterAnimationLayerClass : nullptr;
}

USkeletalMeshComponent *ABBBEquipment::GetEquipmentSkeletalMesh() const
{
    return EquipmentSkeletalMesh;
}

UBBBEquipmentAnimInstance *ABBBEquipment::GetEquipmentAnimationInstance() const
{
    return EquipmentAnimationInstance;
}

void ABBBEquipment::SubmitEquipInput(const int32, const bool)
{
    ensureMsgf(false, TEXT("抽象装备未实现装备输入"));
}

void ABBBEquipment::SubmitPrimaryInput(const int32, const bool)
{
    ensureMsgf(false, TEXT("抽象装备未实现主行为输入"));
}

void ABBBEquipment::SubmitSecondaryInput(const bool, const int32, const bool)
{
    ensureMsgf(false, TEXT("抽象装备未实现副行为输入"));
}

void ABBBEquipment::SubmitReloadInput(const int32, const bool)
{
    ensureMsgf(false, TEXT("抽象装备未实现换弹输入"));
}

void ABBBEquipment::SubmitRestoreFact(const FBBBEquipmentActionFact &, const bool)
{
    ensureMsgf(false, TEXT("抽象装备未实现事实还原输入"));
}

void ABBBEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateEquipment(DeltaSeconds);
}

void ABBBEquipment::UpdateEquipment(const float)
{
}

bool ABBBEquipment::InitializeEquipment(
    UBBBEquipmentDefinition &InDefinition,
    const FGuid &InInstanceId,
    const bool bInIsMirror)
{
    Definition = &InDefinition;
    InstanceId = InInstanceId;
    bIsMirror = bInIsMirror;

    if (!EquipmentSkeletalMesh)
    {
        return false;
    }

    EquipmentSkeletalMesh->SetSkeletalMesh(InDefinition.EquipmentMesh);
    EquipmentSkeletalMesh->SetAnimInstanceClass(InDefinition.EquipmentAnimationClass);
    EquipmentAnimationInstance = Cast<UBBBEquipmentAnimInstance>(EquipmentSkeletalMesh->GetAnimInstance());
    if (!ensureMsgf(
        EquipmentAnimationInstance,
        TEXT("装备 %s 没有创建有效的 UBBBEquipmentAnimInstance"),
        *InDefinition.EquipmentId.ToString()))
    {
        return false;
    }

    return InitializeRuntimeData(InDefinition);
}

bool ABBBEquipment::ValidateInputSource(const bool bInIsMirror) const
{
    return ensureMsgf(
        bInIsMirror == bIsMirror,
        TEXT("装备 %s 收到身份不一致的输入，实例镜像=%d，输入镜像=%d"),
        *GetEquipmentId().ToString(),
        bIsMirror,
        bInIsMirror);
}

bool ABBBEquipment::InitializeRuntimeData(UBBBEquipmentDefinition &)
{
    ensureMsgf(false, TEXT("抽象装备未实现运行时数据初始化"));
    return false;
}
