#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/Core/Initialization/BBBPistolInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/Processors/BBBPistolActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBPistolInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBPistolEquipment &>(BaseEquipment);
    const UBBBPistolDefinition *PistolDefinition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(PistolDefinition, TEXT("手枪必须配置 UBBBPistolDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBPistol] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *PistolDefinition->GetPathName(), PistolDefinition->FireInterval);

    FBBBPistolActionProcessor::Initialize(Equipment.RuntimeData, *PistolDefinition);
    return true;
}
