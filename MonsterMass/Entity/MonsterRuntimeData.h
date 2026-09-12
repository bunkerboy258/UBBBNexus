#pragma once

#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/MonsterMass/Events/MonsterDamageEvent.h"
#include "BBBWork/UBBBNexus/MonsterMass/Events/MonsterDeathEvent.h"
#include "BBBWork/UBBBNexus/MonsterMass/Requests/MonsterTargetRequest.h"
#include "BBBWork/UBBBNexus/MonsterMass/States/MonsterStateData.h"

#include "MonsterRuntimeData.generated.h"

/** 组装小怪运行时叶子数据的配置根 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UMonsterRuntimeData final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 小怪生命配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterHealthFragment Health;

    /** 小怪移动配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterMovementFragment Movement;

    /** 小怪感知配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterPerceptionFragment Perception;

    /** 小怪战斗配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterCombatFragment Combat;

    /** 小怪避让配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterAvoidanceFragment Avoidance;
};
