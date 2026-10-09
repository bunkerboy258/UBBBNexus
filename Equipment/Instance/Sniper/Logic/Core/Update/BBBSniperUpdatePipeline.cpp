#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/Core/Update/BBBSniperUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/Context/BBBSniperUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ParseSystem/BBBSniperParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/BBBSniperActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/BBBSniperAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/NetworkSystem/BBBSniperNetworkSystem.h"

void FBBBSniperUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSniperEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBSniperDefinition *SniperDefinition = Cast<UBBBSniperDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && SniperDefinition && World, TEXT("狙击枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBSniperUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *SniperDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBSniperParseSystem::Update(Context);
    FBBBSniperActionSystem::Update(Context);
    FBBBSniperAnimationSystem::Update(Context);
    FBBBSniperNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
