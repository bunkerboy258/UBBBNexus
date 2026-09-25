#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/BBBRifleParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/BBBRifleActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/BBBRifleAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"

void FBBBRifleUpdatePipeline::Update(ABBBRifleEquipment &Equipment)
{
    if (!Equipment.IsEquipped())
    {
        return;
    }

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && RifleDefinition && World, TEXT("步枪更新缺少必要依赖")))
    {
        FBBBRifleParseProcessor::Clear(Equipment.RuntimeData);
        return;
    }

    // 镜像实例等待第一份完整状态 避免把历史开火误认为刚发生
    if (Equipment.IsMirror() && !Equipment.RuntimeData.Animation.ReadRifleAnimationState().bInitialized
        && !Equipment.RuntimeData.Parse.ReadRifleInputState().AuthorityActionState.bActive)
    {
        return;
    }

    FBBBRifleUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *RifleDefinition,
        Equipment.RuntimeData,
        *World};

    FBBBRifleParseSystem::Update(Context);
    FBBBRifleActionSystem::Update(Context);
    FBBBRifleAnimationSystem::Update(Context);
    FBBBRifleNetworkSystem::Update(Context);
}
