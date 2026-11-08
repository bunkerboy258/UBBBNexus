#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterAnimationUpdateContext;

/** 将角色运行状态转换为动画蓝图可安全读取的事实快照 */
class ABBB_EVAC_API FBBBCharacterAnimationFactProcessor final
{
public:
    /**
     * 采集移动完成后的角色事实
     * @param Character		角色
     * @param RuntimeData	角色运行时数据
     * @param OutFacts		输出事实快照
     * @param DeltaSeconds	本帧间隔
     * @return 无
     */
    /**
     * 采集移动完成后的角色事实
     * @param Context 本次动画更新上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;

};
