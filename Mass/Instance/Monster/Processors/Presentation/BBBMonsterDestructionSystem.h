#pragma once
#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"

class UWorld;
class USkeletalMeshComponent;
class UBBBMonsterGoreDefinition;
struct FBBBMonsterDestructionContribution;
struct FBBBMonsterDamageFragment;
struct FBBBMonsterGoreFragment;

/** 毁伤系统把有效命中转换为八区累计结果 */
struct ABBB_EVAC_API FBBBMonsterDestructionSystem final
{
    /** @param Entity 目标实体 @param World 所在世界 @param Position 接触点 @param Direction 入射方向 @param Strength 已成立强度 @param Result 本玩家当前贡献 @return 是否匹配有效区域 */
    static bool Query(UWorld& World, FMassEntityHandle Entity, const FVector& Position,
        const FVector& Direction, float Strength, FBBBMonsterDestructionContribution& Result);

    /** @param Definition 共享资源 @param Position 世界接触点 @param Direction 世界入射方向 @param MeshTransform 无身体时的参考变换 @param Mesh 可选姿态身体 @return 唯一匹配区域或无 */
    static int32 Select(const UBBBMonsterGoreDefinition& Definition, const FVector& Position,
        const FVector& Direction, const FTransform& MeshTransform, const USkeletalMeshComponent* Mesh);

    /** @param Damage 已合并的玩家贡献 @param Result 当前八区结果 @return 无 */
    static void Resolve(const FBBBMonsterDamageFragment& Damage, FBBBMonsterGoreFragment& Result);
};
