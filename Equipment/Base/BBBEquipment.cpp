#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentStateAuthorityFactPacket.h"
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

bool ABBBEquipment::QueueInput(FBBBEquipmentEquipLocalControlPacket Packet)
{
    ensureMsgf(false, TEXT("抽象装备未实现装备表现输入"));
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet)
{
    ensureMsgf(false, TEXT("抽象装备未实现权威装备表现输入"));
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet)
{
    ensureMsgf(false, TEXT("抽象装备未实现主行为输入"));
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentReloadLocalControlPacket Packet)
{
    ensureMsgf(false, TEXT("抽象装备未实现换弹输入"));
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentStateAuthorityFactPacket Payload)
{
    ensureMsgf(false, TEXT("抽象装备未实现网络状态输入"));
    return false;
}

bool ABBBEquipment::InitializeEquipment()
{
    return FBBBEquipmentInitializer::Initialize(*this);
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
