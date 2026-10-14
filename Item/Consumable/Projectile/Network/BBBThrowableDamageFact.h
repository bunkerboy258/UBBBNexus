#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterPartDamage.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterStaggerContribution.h"
#include "BBBThrowableDamageFact.generated.h"

/** 房主已经裁定的一次爆炸目标结果 */
USTRUCT()
struct FBBBThrowableDamageFact final
{
    GENERATED_BODY()
    UPROPERTY()
    FGuid InstanceId;
    UPROPERTY()
    FBBBMonsterPartDamage Parts;
    UPROPERTY()
    FBBBMonsterStaggerContribution Stagger;
};
