#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBMaodieTestParseState.generated.h"
class APawn;
/** 耄耋Parse状态 */
USTRUCT()
struct FBBBMaodieTestParseState final
{
    GENERATED_BODY()
    /** 待解析的最新事实 */
    UPROPERTY()
    FBBBMaodieTestSnapshot Pending;
    /** 是否有待解析输入 */
    bool bActive = false;
};
