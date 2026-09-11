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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterHealthFragment Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterMovementFragment Movement;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterPerceptionFragment Perception;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterCombatFragment Combat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Runtime")
    FMonsterAvoidanceFragment Avoidance;
};
