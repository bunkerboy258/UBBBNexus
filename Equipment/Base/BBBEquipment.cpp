#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentUnequipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentUnequipAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentActionPermissionLocalControlPacket.h"
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

void ABBBEquipment::BeginPlay()
{
    Super::BeginPlay();

    // 装备自己完成初始化 持有者仅检查结果并管理演员生命周期
    bInitialized = InitializeRuntimeData();
    ensureMsgf(bInitialized, TEXT("装备 %s 初始化失败"), *GetName());
}

FName ABBBEquipment::GetEquipmentId() const
{
    return Definition ? Definition->EquipmentId : NAME_None;
}

TSubclassOf<UAnimInstance> ABBBEquipment::GetCharacterAnimationLayerClass() const
{
    return Definition ? Definition->CharacterAnimationLayerClass : nullptr;
}

bool ABBBEquipment::TryGetAttachmentOffset(FTransform &OutOffset) const
{
    if (!Definition)
    {
        return false;
    }

    OutOffset = Definition->SpawnOffset;
    return true;
}

USkeletalMeshComponent *ABBBEquipment::GetEquipmentSkeletalMesh() const
{
    return EquipmentSkeletalMesh;
}

UBBBEquipmentAnimInstance *ABBBEquipment::GetEquipmentAnimationInstance() const
{
    return EquipmentAnimationInstance;
}

bool ABBBEquipment::TryGetMuzzleTransform(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;
    return false;
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

bool ABBBEquipment::IsMirror() const
{
    const ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    return !Character || Character->IsNetworkMirror();
}

bool ABBBEquipment::IsEquipped() const
{
    const ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    return Character && Character->GetActiveEquipment() == this;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentBeginActionLocalControlPacket Packet)
{
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentEndActionLocalControlPacket Packet)
{
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentBeginContactLocalControlPacket Packet)
{
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentEndContactLocalControlPacket Packet)
{
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentActionPermissionLocalControlPacket Packet)
{
    ensureMsgf(false, TEXT("装备未实现持有者操作许可输入"));
    return false;
}

UBBBEquipmentNetworkComponent *ABBBEquipment::GetNetworkComponent() const
{
    return NetworkComponent;
}

void ABBBEquipment::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bInitialized)
    {
        ShutdownRuntimeData();
    }

    Super::EndPlay(EndPlayReason);
}

bool ABBBEquipment::QueueInput(FBBBEquipmentSecondaryLocalControlPacket Packet)
{
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentUnequipLocalControlPacket Packet)
{
    return false;
}

bool ABBBEquipment::QueueInput(FBBBEquipmentUnequipAuthorityFactPacket Packet)
{
    return false;
}
