#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/Core/Shutdown/BBBPistolShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/Context/BBBPistolUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/Processors/BBBPistolActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ParseSystem/Processors/BBBPistolParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBPistolShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBPistolEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBPistolDefinition *PistolDefinition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBPistolActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && PistolDefinition && World)
    {
        FBBBPistolUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *PistolDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBPistolAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBPistolParseProcessor::Clear(Equipment.RuntimeData);
}
