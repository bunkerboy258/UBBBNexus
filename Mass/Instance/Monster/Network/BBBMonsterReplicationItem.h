#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageContribution.h"
#include "BBBMonsterReplicationItem.generated.h"

class UBBBMonsterDefinition;

/** 单个小怪的当前复制结果 */
USTRUCT()
struct FBBBMonsterReplicationItem final : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    FGuid InstanceId;

    /** 同一代小怪只确定一次的出生种子 */
    UPROPERTY()
    uint32 BirthSeed = 0;

    UPROPERTY()
    uint32 Revision = 0;

    /** 五处当前永久伤口的资源身份 不复制残肢物理或伤害历史 */
    UPROPERTY()
    uint8 DismembermentPresets[5] = {};

    UPROPERTY()
    TObjectPtr<UBBBMonsterDefinition> Definition;

    UPROPERTY()
    FVector_NetQuantize10 Location = FVector::ZeroVector;

    UPROPERTY()
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY()
    FVector_NetQuantize10 Velocity = FVector::ZeroVector;

    UPROPERTY()
    EBBBMonsterBehavior Behavior = EBBBMonsterBehavior::Idle;

    UPROPERTY()
    uint32 ActionId = 0;

    UPROPERTY()
    float StateEnteredTime = 0.0f;

    UPROPERTY()
    TArray<FBBBMonsterDamageContribution> Contributions;
};
