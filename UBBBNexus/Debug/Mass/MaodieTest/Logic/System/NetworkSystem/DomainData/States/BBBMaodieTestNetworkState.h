#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBMaodieTestNetworkState.generated.h"
class APawn;
/** 耄耋Network状态 */
USTRUCT()
struct FBBBMaodieTestNetworkState final
{
    GENERATED_BODY()
    /** 已发布的当前事实 */
    UPROPERTY()
    FBBBMaodieTestSnapshot Published;
};
