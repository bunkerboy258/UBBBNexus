
#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBAimIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBInputIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBItemIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBLocomotionIntentProcessor.h"
struct FBBBCharacterLocomotionConfig;
struct FBBBInputRuntimeData;
struct FBBBIntentRuntimeData;
class FBBBCharacterInitializer;
struct FBBBCharacterWorldRuntimeData;

//根据后处理数据生成意图
class FBBBIntentPipeline final
{
public:

    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    void Initialize(
        FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBInputRuntimeData &InInputData,
        const FBBBCharacterLocomotionConfig &InLocomotionConfig);

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    const FBBBInputRuntimeData *InputData = nullptr;
    FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBCharacterLocomotionConfig *LocomotionConfig = nullptr;

    FBBBInputIntentProcessor InputProcessor;
    FBBBLocomotionIntentProcessor LocomotionProcessor;
    FBBBAimIntentProcessor AimProcessor;
    FBBBEquipmentIntentProcessor EquipmentProcessor;
    FBBBItemIntentProcessor ItemProcessor;
};
