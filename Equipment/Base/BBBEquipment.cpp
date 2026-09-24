#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABBBEquipment::ABBBEquipment()
{
    SetActorEnableCollision(false);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentSkeletalMesh"));
    EquipmentSkeletalMesh->SetupAttachment(EquipmentRoot);
    EquipmentSkeletalMesh->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->PrimaryComponentTick.EndTickGroup = TG_PostUpdateWork;
    EquipmentSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentSkeletalMesh->SetGenerateOverlapEvents(false);
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

void ABBBEquipment::SubmitEquipInput()
{
    ensureMsgf(false, TEXT("抽象装备未实现装备输入"));
}

void ABBBEquipment::SubmitPrimaryInput()
{
    ensureMsgf(false, TEXT("抽象装备未实现主行为输入"));
}

void ABBBEquipment::SubmitReloadInput()
{
    ensureMsgf(false, TEXT("抽象装备未实现换弹输入"));
}

bool ABBBEquipment::InitializeEquipment()
{
    if (!ensureMsgf(
        IsValid(Definition) && !Definition->EquipmentId.IsNone() && EquipmentSkeletalMesh,
        TEXT("装备 %s 缺少有效静态配置、装备标识或骨骼网格"),
        *GetClass()->GetName()))
    {
        return false;
    }

    EquipmentSkeletalMesh->SetSkeletalMesh(Definition->EquipmentMesh);
    EquipmentSkeletalMesh->SetAnimInstanceClass(Definition->EquipmentAnimationClass);
    EquipmentAnimationInstance = Cast<UBBBEquipmentAnimInstance>(EquipmentSkeletalMesh->GetAnimInstance());
    if (!ensureMsgf(
        EquipmentAnimationInstance,
        TEXT("装备 %s 没有创建有效的 UBBBEquipmentAnimInstance"),
        *Definition->EquipmentId.ToString()))
    {
        return false;
    }

    return InitializeRuntimeData();
}

bool ABBBEquipment::InitializeRuntimeData()
{
    ensureMsgf(false, TEXT("抽象装备未实现运行时数据初始化"));
    return false;
}

bool ABBBEquipment::IsMirror() const
{
    const ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    return !Character || Character->RuntimeData.External.ReadNetworkIdentityState().bIsMirror;
}

bool ABBBEquipment::IsEquipped() const
{
    const ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    return Character && Character->GetActiveEquipment() == this;
}

void ABBBEquipment::OnUnequipped()
{
    ensureMsgf(false, TEXT("装备未实现卸下清理"));
}

bool ABBBEquipment::SubmitNetworkPayload(const TArray<uint8> &Data)
{
    ensureMsgf(false, TEXT("装备未实现网络状态输入"));
    return false;
}
