#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBRifleInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRifleEquipment &>(BaseEquipment);
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(RifleDefinition, TEXT("步枪必须配置 UBBBRifleDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBRifle] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *RifleDefinition->GetPathName(), RifleDefinition->FireInterval);

    FBBBRifleActionProcessor::Initialize(Equipment.RuntimeData, *RifleDefinition);
    return true;
}
