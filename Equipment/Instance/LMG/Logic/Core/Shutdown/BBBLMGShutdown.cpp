#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/Core/Shutdown/BBBLMGShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/Context/BBBLMGUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/Processors/BBBLMGActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ParseSystem/Processors/BBBLMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBLMGShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBLMGEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBLMGDefinition *LMGDefinition = Cast<UBBBLMGDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBLMGActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && LMGDefinition && World)
    {
        FBBBLMGUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *LMGDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBLMGAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBLMGParseProcessor::Clear(Equipment.RuntimeData);
}
