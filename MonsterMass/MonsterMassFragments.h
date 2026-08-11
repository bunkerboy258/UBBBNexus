#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "MonsterMassFragments.generated.h"

/** 小怪当前的核心状态 */
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    /** 空闲 */
    Idle,

    /** 追逐目标 */
    Chase,

    /** 攻击目标 */
    Attack,

    /** 已死亡 */
    Dead
};

/** 标记实体属于小怪 */
USTRUCT()
struct ABBB_EVAC_API FMonsterTag final : public FMassTag
{
    GENERATED_BODY()
};

/** 小怪生命值数据 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterHealthFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float CurrentHealth = 100.0f;

    /** 最大生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float MaxHealth = 100.0f;
};

/** 小怪状态数据 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterStateFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前状态 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    EMonsterState State = EMonsterState::Chase;
};

/** 小怪目标数据 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterTargetFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 目标世界坐标 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    FVector TargetLocation = FVector::ZeroVector;
};

/** 小怪移动参数 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterMovementFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 移动速度，单位为厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float MoveSpeed = 300.0f;

    /** 停止半径，单位为厘米 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float StopRadius = 80.0f;
};
