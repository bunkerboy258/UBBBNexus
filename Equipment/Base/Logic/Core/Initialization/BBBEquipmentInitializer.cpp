#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentDefinition.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBEquipmentInitializer::Initialize(ABBBEquipment &Equipment)
{
    if (!ensureMsgf(
        IsValid(Equipment.Definition) && !Equipment.Definition->EquipmentId.IsNone()
            && Equipment.EquipmentSkeletalMesh,
        TEXT("装备 %s 缺少有效静态配置、装备标识或骨骼网格"),
        *Equipment.GetClass()->GetName()))
    {
        return false;
    }

    Equipment.EquipmentSkeletalMesh->SetSkeletalMesh(Equipment.Definition->EquipmentMesh);
    Equipment.EquipmentSkeletalMesh->SetAnimInstanceClass(Equipment.Definition->EquipmentAnimationClass);
    Equipment.EquipmentAnimationInstance = Cast<UBBBEquipmentAnimInstance>(
        Equipment.EquipmentSkeletalMesh->GetAnimInstance());
    if (!ensureMsgf(
        Equipment.EquipmentAnimationInstance,
        TEXT("装备 %s 没有创建有效的 UBBBEquipmentAnimInstance"),
        *Equipment.Definition->EquipmentId.ToString()))
    {
        return false;
    }

    return Equipment.InitializeRuntimeData();
}
