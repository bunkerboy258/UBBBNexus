#pragma once

#include "MassEntityTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "BBBMonsterGroundFragment.generated.h"

class UPrimitiveComponent;

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

    /** 最近一次求解确认的静态支撑 */
    TWeakObjectPtr<UPrimitiveComponent> StaticSupport;

    /** 确认支撑时的组件变换与胶囊中心 */
    FTransform StaticSupportTransform = FTransform::Identity;
    FVector StaticSupportCenter = FVector::ZeroVector;

    /** 确认静态支撑时使用的胶囊尺寸 */
    float StaticSupportRadius = 0.0f;
    float StaticSupportHalfHeight = 0.0f;

    /** 静态支撑是否来自连续移动的最后一次落脚查询 */
    bool bStaticSupportFromMovement = false;

    /** 下次远处静止支撑查询的世界时间 */
    double NextSupportCheckTime = 0.0;

    /** 按最近玩家距离维持的远处判定 */
    bool bFarFromPlayers = false;

    /** 静态导航走廊中等待合并执行的地面求解时间 */
    float DeferredMovementSeconds = 0.0f;
};
