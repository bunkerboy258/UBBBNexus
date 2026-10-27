#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterGroundFragment.generated.h"

/** 由移动处理器维护的地面支撑结果 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterGroundFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前逻辑胶囊是否获得可行走地面支撑 */
    bool bGrounded = false;

    /** 当前支撑面的法线 */
    FVector SupportNormal = FVector::UpVector;

    /** 移动求解器上次使用的胶囊半高 首帧由静态配置初始化 */
    float CapsuleHalfHeight = 0.0f;

    /** 当前地面求解收到的水平移动意图长度 */
    float RequestedDistance = 0.0f;

    /** 当前地面求解完成的水平位移长度 */
    float MovedDistance = 0.0f;

    /** 有限积累的未模拟时间 防止长帧触发无限补算 */
    float UnsimulatedSeconds = 0.0f;

    /** 连续失去真实支撑的开始时刻 有支撑时为负 */
    float UnsupportedSince = -1.0f;
};
