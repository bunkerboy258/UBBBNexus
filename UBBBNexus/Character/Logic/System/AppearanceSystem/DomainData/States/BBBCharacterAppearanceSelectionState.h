#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceSnapshot.h"
#include "BBBCharacterAppearanceSelectionState.generated.h"
/** 已经成立且允许公开观察的外观结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceSelectionState final
{
    GENERATED_BODY()
    /** 网络与表现共同读取的完整事实 */
    UPROPERTY(BlueprintReadOnly)
    FBBBCharacterAppearanceSnapshot Snapshot;
};
