#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/Core/Update/BBBSMGUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/Context/BBBSMGUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/RuntimeData/BBBSMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/BBBSMGParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/BBBSMGActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/BBBSMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/NetworkSystem/BBBSMGNetworkSystem.h"

void FBBBSMGUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSMGEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBSMGDefinition *SMGDefinition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && SMGDefinition && World, TEXT("冲锋枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBSMGUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *SMGDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBSMGParseSystem::Update(Context);
    FBBBSMGActionSystem::Update(Context);
    FBBBSMGAnimationSystem::Update(Context);
    FBBBSMGNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
