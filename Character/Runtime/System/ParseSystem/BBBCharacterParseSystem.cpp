#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterParseSystem::Initialize(
    FBBBCharacterRuntimeData &InData,
    UBBBEquipmentCatalog &InEquipmentCatalog,
    USkeletalMeshComponent &InCharacterMesh)
{
    Data = &InData;
    EquipmentCatalog = &InEquipmentCatalog;
    CharacterMesh = &InCharacterMesh;
}

void FBBBCharacterParseSystem::Update() const
{
    if (!Data || !EquipmentCatalog || !CharacterMesh)
    {
        return;
    }

    UBBBAnimInstance *AnimationInstance = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    ensureMsgf(AnimationInstance, TEXT("角色输入解析缺少 BBB 动画实例 蒙太奇输入将在本帧被拒绝"));

    FBBBCharacterInputContext Context{
        Data->Parse.OperationState,
        Data->Equipment.EquipmentInventoryState,
        Data->Equipment.EquipmentSelectionState,
        Data->Equipment.EquipmentCommandState,
        Data->Equipment.EquipmentEventState,
        AnimationInstance,
        Data->Aim.AimState,
        Data->Locomotion.LocomotionState,
        Data->Parse.ControlState,
        Data->Parse.CameraState.PendingInput,
        *EquipmentCatalog};
    InputProcessor.Update(Data->Parse.InputState, Context);
}
