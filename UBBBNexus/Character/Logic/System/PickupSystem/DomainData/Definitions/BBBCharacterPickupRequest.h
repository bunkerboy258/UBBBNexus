#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterPickupRequest.generated.h"
class ABBBWorldDropItem;
/** 角色请求领取的世界对象身份 */
USTRUCT()
struct FBBBCharacterPickupRequest final
{
    GENERATED_BODY()
    /** 请求领取的世界对象 */
    UPROPERTY()
    TObjectPtr<ABBBWorldDropItem> Drop = nullptr;
    /** 防止旧请求作用于其它掉落物 */
    UPROPERTY()
    FGuid DropId;
};
