#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/Core/Update/BBBMinigunUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/Context/BBBMinigunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ParseSystem/BBBMinigunParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/BBBMinigunActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/BBBMinigunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/NetworkSystem/BBBMinigunNetworkSystem.h"

void FBBBMinigunUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMinigunEquipment &>(BaseEquipment);

    ABBBCharacter *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    USkeletalMeshComponent *Mesh = Equipment.GetEquipmentSkeletalMesh();
    const UBBBMinigunDefinition *MinigunDefinition = Cast<UBBBMinigunDefinition>(Equipment.GetDefinition());
    UWorld *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Mesh && MinigunDefinition && World, TEXT("转管机枪更新缺少必要依赖")))
    {
        return;
    }

    FBBBMinigunUpdateContext Context{
        Equipment,
        *Character,
        *Mesh,
        *MinigunDefinition,
        Equipment.RuntimeData,
        *World, !Equipment.IsMirror()};

    FBBBMinigunParseSystem::Update(Context);
    FBBBMinigunActionSystem::Update(Context);
    FBBBMinigunAnimationSystem::Update(Context);
    FBBBMinigunNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
