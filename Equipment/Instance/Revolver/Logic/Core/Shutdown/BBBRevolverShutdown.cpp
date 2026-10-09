#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/Core/Shutdown/BBBRevolverShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/Context/BBBRevolverUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/Processors/BBBRevolverActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/Processors/BBBRevolverAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ParseSystem/Processors/BBBRevolverParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBRevolverShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRevolverEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBRevolverDefinition *RevolverDefinition = Cast<UBBBRevolverDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBRevolverActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && RevolverDefinition && World)
    {
        FBBBRevolverUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *RevolverDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBRevolverAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBRevolverParseProcessor::Clear(Equipment.RuntimeData);
}
