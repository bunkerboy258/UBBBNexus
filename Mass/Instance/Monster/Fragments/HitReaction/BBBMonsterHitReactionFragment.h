#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterHitRegion.h"
#include "BBBMonsterHitReactionFragment.generated.h"

/** 最近一次成立命中的表现事实 不保存命中历史 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterHitReactionFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 命中部位 */
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;
    /** 世界接触点 */
    FVector Position = FVector::ZeroVector;
    /** 世界飞行方向 */
    FVector Direction = FVector::ForwardVector;
    /** 世界表面法线 */
    FVector Normal = FVector::UpVector;
    /** 最近命中之后的秒数 */
    float Age = 10.0f;
    /** 本地命中编号 镜像子弹也可推进表现 */
    uint32 Serial = 0;
    /** 已提交光效的最新编号 */
    uint32 PublishedSerial = 0;
};
