#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/Core/Update/BBBRevolverUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/Context/BBBRevolverUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/RuntimeData/BBBRevolverRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ParseSystem/BBBRevolverParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/BBBRevolverActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/BBBRevolverAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/NetworkSystem/BBBRevolverNetworkSystem.h"

void FBBBRevolverUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRevolverEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBRevolverDefinition *RevolverDefinition = Cast<UBBBRevolverDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && RevolverDefinition && World, TEXT("左轮更新缺少必要依赖")))
    {
        return;
    }

    FBBBRevolverUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *RevolverDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBRevolverParseSystem::Update(Context);
    FBBBRevolverActionSystem::Update(Context);
    FBBBRevolverAnimationSystem::Update(Context);
    FBBBRevolverNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
