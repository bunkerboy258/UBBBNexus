#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/Core/Shutdown/BBBSniperShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/Context/BBBSniperUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/Processors/BBBSniperActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ParseSystem/Processors/BBBSniperParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBSniperShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSniperEquipment &>(BaseEquipment);
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBSniperDefinition *SniperDefinition = Cast<UBBBSniperDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBSniperActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && SniperDefinition && World)
    {
        FBBBSniperUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *SniperDefinition,
            Equipment.RuntimeData,
            *World, !Equipment.IsMirror()};
        FBBBSniperAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBSniperParseProcessor::Clear(Equipment.RuntimeData);
}
