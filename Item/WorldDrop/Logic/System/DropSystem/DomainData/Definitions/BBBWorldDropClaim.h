#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
#include "BBBWorldDropClaim.generated.h"
/** 一次共享世界掉落物领取请求 */
USTRUCT()
struct FBBBWorldDropClaim final
{
    GENERATED_BODY()
    /** 发起领取的角色 */
    UPROPERTY()
    TObjectPtr<ABBBCharacter> Character = nullptr;
    /** 请求指向的掉落物身份 */
    UPROPERTY()
    FGuid DropId;

};
