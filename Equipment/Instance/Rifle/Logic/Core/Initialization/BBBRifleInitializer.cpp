#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Initialization/BBBRifleInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"

bool FBBBRifleInitializer::Initialize(ABBBRifleEquipment &Equipment)
{
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(RifleDefinition, TEXT("步枪必须配置 UBBBRifleDefinition")))
    {
        return false;
    }

    FBBBRifleActionProcessor::Initialize(Equipment.RuntimeData, *RifleDefinition);
    return true;
}
