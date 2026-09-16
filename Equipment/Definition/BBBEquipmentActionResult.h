#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentActionPresentation.h"

/** 装备操作向角色公开的成功结果 */
struct FBBBEquipmentActionResult
{
    float DurationSeconds = 0.0f;
    FBBBEquipmentActionPresentation Presentation;
    FVector2D RecoilImpulse = FVector2D::ZeroVector;
    float RecoilRecoverySpeed = 0.0f;
};
