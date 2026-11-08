#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimTargetProcessor.h"

class FBBBCharacterInitializer;
struct FBBBAimDomainState;
struct FBBBCharacterControlState;
struct FBBBCharacterLifeState;

/**
 * 按角色瞄准状态分流各个瞄准处理器
 */
class ABBB_EVAC_API FBBBCharacterAimSystem final
{
public:
    /**
     * 按固定顺序更新瞄准状态与目标
     */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化瞄准系统依赖
     * @param InAimData    瞄准运行数据
     * @param InIntentData 角色意图数据
     */
    void Initialize(
        FBBBAimDomainState &InAimData,
        const FBBBCharacterControlState &InIntentData,
        const FBBBCharacterLifeState &InLife);

    FBBBAimDomainState *AimData = nullptr;
    const FBBBCharacterControlState *ControlData = nullptr;
    const FBBBCharacterLifeState *Life = nullptr;

    FBBBCharacterAimStateProcessor AimStateProcessor;
    FBBBCharacterAimTargetProcessor AimTargetProcessor;
};
