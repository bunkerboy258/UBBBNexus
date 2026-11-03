#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBMaodieTestAnimationState.generated.h"
class APawn;
/** 耄耋Animation状态 */
USTRUCT()
struct FBBBMaodieTestAnimationState final
{
    GENERATED_BODY()
    /** 上次播放的动作 */
    uint8 LastPhase = 255;
    /** 已播放的哈气序号 */
    uint32 LastHissSerial = 0;
};
