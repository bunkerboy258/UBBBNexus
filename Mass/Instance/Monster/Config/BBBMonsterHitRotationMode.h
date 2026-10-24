#pragma once

#include "BBBMonsterHitRotationMode.generated.h"

/** 有界的受击旋转方向规则 */
UENUM(BlueprintType)
enum class EBBBMonsterHitRotationMode : uint8
{
    /** 使用命中位置相对施力骨骼的力臂 */
    ContactTorque UMETA(DisplayName = "命中位置扭矩"),
    /** 按入射方向形成稳定的躯干偏转 */
    BodyDeflection UMETA(DisplayName = "稳定躯干偏转"),
    /** 按入射方向形成稳定的四肢偏转 */
    LimbDeflection UMETA(DisplayName = "稳定四肢偏转")
};
