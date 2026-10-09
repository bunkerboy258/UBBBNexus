#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/Core/Initialization/BBBSniperInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/Processors/BBBSniperActionProcessor.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBSniperInitializer::InitializeInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSniperEquipment &>(BaseEquipment);
    const UBBBSniperDefinition *SniperDefinition = Cast<UBBBSniperDefinition>(Equipment.GetDefinition());
    if (!ensureMsgf(SniperDefinition, TEXT("狙击枪必须配置 UBBBSniperDefinition")))
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBSniper] Initialized Equipment=%s Definition=%s FireInterval=%.3f"),
        *Equipment.GetName(), *SniperDefinition->GetPathName(), SniperDefinition->FireInterval);

    FBBBSniperActionProcessor::Initialize(Equipment.RuntimeData, *SniperDefinition);
    return true;
}
