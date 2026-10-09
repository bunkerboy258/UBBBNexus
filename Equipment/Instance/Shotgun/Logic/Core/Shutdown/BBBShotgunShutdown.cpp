#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/Core/Shutdown/BBBShotgunShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/Processors/BBBShotgunActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/Processors/BBBShotgunAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ParseSystem/Processors/BBBShotgunParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBShotgunShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBShotgunEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBShotgunDefinition *ShotgunDefinition = Cast<UBBBShotgunDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBShotgunActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && ShotgunDefinition && World)
    {
        FBBBShotgunUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *ShotgunDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBShotgunAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBShotgunParseProcessor::Clear(Equipment.RuntimeData);
}
