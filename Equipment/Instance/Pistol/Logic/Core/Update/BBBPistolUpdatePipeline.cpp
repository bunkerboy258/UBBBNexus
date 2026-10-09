#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/Core/Update/BBBPistolUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/Context/BBBPistolUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ParseSystem/BBBPistolParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/BBBPistolActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/BBBPistolAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/NetworkSystem/BBBPistolNetworkSystem.h"

void FBBBPistolUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBPistolEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBPistolDefinition *PistolDefinition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && PistolDefinition && World, TEXT("手枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBPistolUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *PistolDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBPistolParseSystem::Update(Context);
    FBBBPistolActionSystem::Update(Context);
    FBBBPistolAnimationSystem::Update(Context);
    FBBBPistolNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
