#pragma once
#include "CoreMinimal.h"
class UBBBMonsterGoreDefinition;
class USkeletalMeshComponent;
struct FBBBMonsterGoreFragment;
struct FBBBMonsterHitReactionFragment;

/** 断肢系统把已成立的部位损毁选择为稳定的预制断点 */
struct ABBB_EVAC_API FBBBMonsterDismembermentSystem final
{
    /** @param Result 当前断点 @param Definition 共享资源 @param DestroyedParts 已成立部位损毁 @param Hit 当前命中 @param Mesh 可选姿态身体 @return 无 */
    static void Resolve(FBBBMonsterGoreFragment& Result, const UBBBMonsterGoreDefinition& Definition,
        uint8 DestroyedParts, const FBBBMonsterHitReactionFragment& Hit, const USkeletalMeshComponent* Mesh);
};
