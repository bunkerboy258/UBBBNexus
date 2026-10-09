#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/Core/Shutdown/BBBSMGShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/Context/BBBSMGUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/Processors/BBBSMGActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/Processors/BBBSMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBSMGShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSMGEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBSMGDefinition *SMGDefinition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBSMGActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && SMGDefinition && World)
    {
        FBBBSMGUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *SMGDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBSMGAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBSMGParseProcessor::Clear(Equipment.RuntimeData);
}
