#pragma once
#include "CoreMinimal.h"
class UBBBThrowableDefinition;
/** 初始化和引信共享生命周期 */
struct FBBBThrowableLifetimeState final
{
    /** 当前是否已释放 */
    bool bLaunched = false;
    /** 释放时世界时间 */
    double LaunchTime = 0.0;
    /** 等待初始化的静态定义 */
    UBBBThrowableDefinition* PendingDefinition = nullptr;
    /** 等待初始化的释放变换 */
    FTransform PendingTransform = FTransform::Identity;
    /** 释放时持有者速度 */
    FVector PendingVelocity = FVector::ZeroVector;
};
