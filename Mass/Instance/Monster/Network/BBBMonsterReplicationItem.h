#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBMonsterReplicationItem.generated.h"

class UBBBMonsterDefinition;

/** 单个小怪的当前复制结果 */
USTRUCT()
struct FBBBMonsterReplicationItem final : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    FGuid InstanceId;

    UPROPERTY()
    uint32 Revision = 0;

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
    float Health = 0.0f;
};
