#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/Core/Shutdown/BBBMinigunShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/Context/BBBMinigunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/Processors/BBBMinigunActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ParseSystem/Processors/BBBMinigunParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBMinigunShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMinigunEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBMinigunDefinition *MinigunDefinition = Cast<UBBBMinigunDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBMinigunActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && MinigunDefinition && World)
    {
        FBBBMinigunUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *MinigunDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBMinigunAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBMinigunParseProcessor::Clear(Equipment.RuntimeData);
}
