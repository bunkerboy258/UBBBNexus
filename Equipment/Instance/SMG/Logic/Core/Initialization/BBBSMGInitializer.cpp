#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/Core/Initialization/BBBSMGInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/Processors/BBBSMGActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBSMGInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSMGEquipment &>(BaseEquipment);
    const UBBBSMGDefinition *SMGDefinition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(SMGDefinition, TEXT("冲锋枪必须配置 UBBBSMGDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBSMG] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *SMGDefinition->GetPathName(), SMGDefinition->FireInterval);

    FBBBSMGActionProcessor::Initialize(Equipment.RuntimeData, *SMGDefinition);
    return true;
}
