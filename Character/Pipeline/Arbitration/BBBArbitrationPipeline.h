
#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterActionDomainArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterActionPriorityArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterEquipmentStateArbitrator.h"
class FBBBCharacterInitializer;
struct FBBBCharacterEquipmentState;
struct FBBBDecisionRuntimeData;

//依次应用装备状态动作域与优先级规则
class FBBBArbitrationPipeline final
{
public:

    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        const FBBBCharacterEquipmentState &InEquipmentState);

    FBBBDecisionRuntimeData *DecisionData = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;

    FBBBCharacterEquipmentStateArbitrator EquipmentStateArbitrator;
    FBBBCharacterActionDomainArbitrator ActionDomainArbitrator;
    FBBBCharacterActionPriorityArbitrator ActionPriorityArbitrator;
    
};
