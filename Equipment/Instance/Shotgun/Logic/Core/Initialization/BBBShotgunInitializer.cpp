#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/Core/Initialization/BBBShotgunInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/Processors/BBBShotgunActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBShotgunInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBShotgunEquipment &>(BaseEquipment);
    const UBBBShotgunDefinition *ShotgunDefinition = Cast<UBBBShotgunDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(ShotgunDefinition, TEXT("霰弹枪必须配置 UBBBShotgunDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBShotgun] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *ShotgunDefinition->GetPathName(), ShotgunDefinition->FireInterval);

    FBBBShotgunActionProcessor::Initialize(Equipment.RuntimeData, *ShotgunDefinition);
    return true;
}
