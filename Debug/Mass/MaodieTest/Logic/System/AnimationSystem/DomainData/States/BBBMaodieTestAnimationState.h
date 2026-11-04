#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBMaodieTestAnimationState.generated.h"
class UMaterialInstanceDynamic;
class UAudioComponent;
/** 耄耋Animation状态 */
USTRUCT()
struct FBBBMaodieTestAnimationState final
{
    GENERATED_BODY()
    /** 本实例独占的表情材质 */
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> ExpressionMaterial;
    /** 正在播放的哈气声源 */
    UPROPERTY()
    TObjectPtr<UAudioComponent> HissAudio;
    /** 平滑后的头部附加角度 */
    FRotator HeadRotation = FRotator::ZeroRotator;
    /** 上次播放的动作 */
    uint8 LastPhase = 255;
    /** 已播放的哈气序号 */
    uint32 LastHissSerial = 0;
};
