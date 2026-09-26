#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Shutdown/BBBRifleShutdown.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBRifleShutdown::Shutdown(ABBBRifleEquipment &Equipment)
{
    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    FBBBRifleActionProcessor::Stop(Equipment.RuntimeData);
    if (Character && Mesh && RifleDefinition && World)
    {
        FBBBRifleUpdateContext Context{
            Equipment,
            *Character,
            *Mesh,
            *RifleDefinition,
            Equipment.RuntimeData,
            *World};
        FBBBRifleAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBRifleParseProcessor::Clear(Equipment.RuntimeData);
}
