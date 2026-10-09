#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"
#include "BBBProjectileCollisionFragment.generated.h"

/** 保存单枚实体弹丸运行期间所需的弹道与命中数据 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectileCollisionFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前命中伤害 */
    float Damage = 0.0f;

    /** 穿透衰减后的当前耐久伤害 */
    float DurableDamage = 0.0f;

    /** 范围伤害半径 零表示点伤害 */
    float ExplosionRadiusCm = 0.0f;

    /** 接触目标时是否引爆 */
    bool bDetonateOnImpact = true;

    /** 接触后是否反弹 */
    bool bBounceOnImpact = false;

    /** 反弹速度倍率 */
    float BounceRestitution = 0.4f;

    /** 每次穿透后应用的伤害倍率 */
    float PenetrationDamageMultiplier = 1.0f;

    /** 剩余可穿透目标数量 */
    int32 RemainingPenetrations = 0;

    /** 连续碰撞检测半径 单位厘米 */
    float CollisionRadiusCm = 0.0f;

    /** 执行连续碰撞查询的碰撞通道 */
    ECollisionChannel CollisionChannel = ECC_Pawn;

    /** 开火装备 用于伤害来源与碰撞忽略 */
    TWeakObjectPtr<AActor> DamageCauser;

    /** 发射弹丸的角色 用于碰撞忽略 */
    TWeakObjectPtr<APawn> InstigatorPawn;

    /** 伤害事件中的控制器来源 */
    TWeakObjectPtr<AController> EventInstigator;

    /** 已命中过的最近目标 防止穿透期间重复命中 */
    TWeakObjectPtr<AActor> LastHitActor;

    /** 仅来源在本机控制时允许产生伤害 */
    bool bCanCauseDamage = false;

    /** 最近穿透的逻辑实体 */
    FMassEntityHandle LastHitEntity;
};
