
#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBAimIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentActionIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBInputIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentSelectionIntentProcessor.h"
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

    /**
     * 依次执行全部意图处理器并发布本帧意图快照
     */
    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    /**
     * 注入意图管线固定依赖
     * @param InIntentData	意图运行时数据
     * @param InWorldData	角色所属世界运行时数据
     * @param InInputData	输入后处理数据
     * @param InLocomotionConfig	角色移动配置
     */
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
    FBBBEquipmentActionIntentProcessor ActionProcessor;
    FBBBEquipmentSelectionIntentProcessor SelectionProcessor;
};
