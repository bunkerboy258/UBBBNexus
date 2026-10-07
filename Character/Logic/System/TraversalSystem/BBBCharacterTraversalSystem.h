#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/Processors/BBBCharacterTraversalProbeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/Processors/BBBCharacterTraversalLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/Processors/BBBCharacterTraversalWarpProcessor.h"

class ABBBCharacter;
class UMotionWarpingComponent;
struct FBBBCharacterRuntimeData;
struct FBBBTraversalConfig;
class FBBBCharacterInitializer;

/** 维护攀爬业务与目标 不承担角色移动或动画播放 */
class ABBB_EVAC_API FBBBCharacterTraversalSystem final
{
public:
    /** @return 无 按固定顺序更新检测 生命周期与校正目标 */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * @param InCharacter	动作所属角色
     * @param InData	角色唯一运行时黑板
     * @param InConfig	攀爬配置
     * @param InWarping	官方根运动校正组件
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData,
        const FBBBTraversalConfig &InConfig, UMotionWarpingComponent &InWarping);

    /** 初始化后由角色持有的系统依赖 */
    ABBBCharacter *Character = nullptr;
    /** 所有跨帧业务结果写入此黑板所属领域 */
    FBBBCharacterRuntimeData *Data = nullptr;
    /** 角色资产提供的检测与超时配置 */
    const FBBBTraversalConfig *Config = nullptr;
    /** 仅管理攀爬命名目标与对应校正窗口 */
    UMotionWarpingComponent *Warping = nullptr;

    FBBBCharacterTraversalProbeProcessor ProbeProcessor;
    FBBBCharacterTraversalLifeProcessor LifeProcessor;
    FBBBCharacterTraversalWarpProcessor WarpProcessor;
};
