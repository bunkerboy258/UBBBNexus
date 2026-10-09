#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/Context/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/BBBRifleParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/BBBRifleActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/BBBRifleAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"

void FBBBRifleUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRifleEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && RifleDefinition && World, TEXT("步枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBRifleUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *RifleDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBRifleParseSystem::Update(Context);
    FBBBRifleActionSystem::Update(Context);
    FBBBRifleAnimationSystem::Update(Context);
    FBBBRifleNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
