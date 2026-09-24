#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterItem.generated.h"

class AActor;

/** 角色持有的单件物品及其运行时实体 */
USTRUCT(BlueprintType)
struct FBBBCharacterItem final
{
    GENERATED_BODY()

    /** 物品的运行时演员 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> ItemActor = nullptr;
};
