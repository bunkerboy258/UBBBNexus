#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/Core/Update/BBBLMGUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/Context/BBBLMGUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ParseSystem/BBBLMGParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/BBBLMGActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/BBBLMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/NetworkSystem/BBBLMGNetworkSystem.h"

void FBBBLMGUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBLMGEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBLMGDefinition *LMGDefinition = Cast<UBBBLMGDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && LMGDefinition && World, TEXT("轻机枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBLMGUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *LMGDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBLMGParseSystem::Update(Context);
    FBBBLMGActionSystem::Update(Context);
    FBBBLMGAnimationSystem::Update(Context);
    FBBBLMGNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
