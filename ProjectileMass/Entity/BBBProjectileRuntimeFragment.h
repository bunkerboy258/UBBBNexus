#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "MassEntityTypes.h"
#include "BBBProjectileRuntimeFragment.generated.h"

/** 保存单枚实体弹丸运行期间所需的弹道与命中数据 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectileRuntimeFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 本帧移动开始前的位置 */
    FVector PreviousLocation = FVector::ZeroVector;

    /** 剩余存活时间，单位秒 */
    float RemainingLifetimeSeconds = 0.0f;

    /** 当前命中伤害 */
    float Damage = 0.0f;

    /** 每次穿透后应用的伤害倍率 */
    float PenetrationDamageMultiplier = 1.0f;

    /** 剩余可穿透目标数量 */
    int32 RemainingPenetrations = 0;

    /** 连续碰撞检测半径，单位厘米 */
    float CollisionRadiusCm = 0.0f;

    /** 执行连续碰撞查询的碰撞通道 */
    ECollisionChannel CollisionChannel = ECC_Pawn;

    /** 开火装备，用于伤害来源与碰撞忽略 */
    TWeakObjectPtr<AActor> DamageCauser;

    /** 发射弹丸的角色，用于碰撞忽略 */
    TWeakObjectPtr<APawn> InstigatorPawn;

    /** 伤害事件中的控制器来源 */
    TWeakObjectPtr<AController> EventInstigator;

    /** 已命中过的最近目标，防止穿透期间重复命中 */
    TWeakObjectPtr<AActor> LastHitActor;

    /** 碰撞或寿命处理器请求回收该弹丸 */
    bool bPendingDestroy = false;
};
