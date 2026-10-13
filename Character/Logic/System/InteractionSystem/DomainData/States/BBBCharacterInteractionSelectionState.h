#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBCharacterInteractionSelectionState.generated.h"

/** 角色已经成立的实际主手关系 */
USTRUCT(BlueprintType)
struct FBBBCharacterInteractionSelectionState final
{
    GENERATED_BODY()
    /** 实际持有物品的型号 */
    FName ActiveItemId;
    /** 唯一实际持有实例 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "当前主手物品"))
    TObjectPtr<AActor> ActiveMainHandInstance = nullptr;
    /** 待还原的目标型号 空标识表示空手 */
    FName PendingItemId;
    /** 是否收到持有结果 */
    bool bHasInteractionRequest = false;
    /** 当前实际持有代次 */
    uint64 ActiveGeneration = 1;
    /** 待还原代次 */
    uint64 PendingGeneration = 0;
};
