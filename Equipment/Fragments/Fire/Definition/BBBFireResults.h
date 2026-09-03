#pragma once

#include "CoreMinimal.h"
#include "BBBFireResults.generated.h"

/** 单次成功开火产生的角色反馈 */
USTRUCT(BlueprintType)
struct FBBBEquipmentFireResult
{
    GENERATED_BODY()

    /** 后坐力二维冲量 */
    UPROPERTY(BlueprintReadOnly)
    FVector2D RecoilImpulse = FVector2D::ZeroVector;

    /** 后坐力恢复速度 */
    UPROPERTY(BlueprintReadOnly)
    float RecoilRecoverySpeed = 0.0f;
};
