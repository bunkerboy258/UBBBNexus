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
};
