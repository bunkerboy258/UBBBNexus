#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemUseState.generated.h"
class UBBBConsumable;
/** 仅在一次使用期间拥有运行时对象 */
USTRUCT()
struct FBBBCharacterItemUseState final
{
    GENERATED_BODY()
    /** 本次使用的唯一强引用 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBConsumable> Runtime = nullptr;
    /** 使用中的库存实例 */
    FGuid Instance;
    /** 已交接的使用序号 */
    int32 Serial = 0;
};
