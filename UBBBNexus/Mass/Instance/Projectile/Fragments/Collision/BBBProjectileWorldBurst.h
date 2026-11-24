#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"

/** 同批破片共享的世界命中记录 由碰撞处理器一次性提交 */
struct FBBBProjectileWorldBurst final
{
    int32 PlayerId = INDEX_NONE;
    bool bDamagePlayers = true;
    TMap<FMassEntityHandle, TArray<FBBBHitContribution>> EntityHits;
    TMap<TWeakObjectPtr<AActor>, TArray<FBBBHitContribution>> ActorHits;
};
