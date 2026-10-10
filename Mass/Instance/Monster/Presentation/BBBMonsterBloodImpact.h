#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"

/** 单轮批量血效的原始接触事实 */
struct FBBBMonsterBloodImpact final
{
    FVector Position = FVector::ZeroVector;
    FVector Direction = FVector::ForwardVector;
    FVector Normal = FVector::UpVector;
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;
    /** 当前命中编号只参与表现随机化 不保存编号列表 */
    uint32 Seed = 0;
    /** 当前断肢瞬间增加定向喷射与短促滴落 不保存事件历史 */
    bool bSevering = false;
};
