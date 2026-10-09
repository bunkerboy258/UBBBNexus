#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/Core/Update/BBBShotgunUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ParseSystem/BBBShotgunParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/BBBShotgunActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/BBBShotgunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/BBBShotgunNetworkSystem.h"

void FBBBShotgunUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBShotgunEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBShotgunDefinition *ShotgunDefinition = Cast<UBBBShotgunDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && ShotgunDefinition && World, TEXT("霰弹枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBShotgunUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *ShotgunDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBShotgunParseSystem::Update(Context);
    FBBBShotgunActionSystem::Update(Context);
    FBBBShotgunAnimationSystem::Update(Context);
    FBBBShotgunNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
