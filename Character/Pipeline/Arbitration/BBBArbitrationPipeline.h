
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

    /**
     * 依次应用装备状态动作域与优先级仲裁
     */
    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    /**
     * 注入仲裁管线固定依赖
     * @param InDecisionData	角色仲裁数据
     * @param InEquipmentState	角色装备状态
     */
    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        const FBBBCharacterEquipmentState &InEquipmentState);

    FBBBDecisionRuntimeData *DecisionData = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;

    FBBBCharacterEquipmentStateArbitrator EquipmentStateArbitrator;
    FBBBCharacterActionDomainArbitrator ActionDomainArbitrator;
    FBBBCharacterActionPriorityArbitrator ActionPriorityArbitrator;
    
};
