#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBLinkedAnimInstance.generated.h"

class UBBBAnimInstance;

/** 链接动画层访问主动画实例的线程安全基类 */
UCLASS(Abstract)
class ABBB_EVAC_API UBBBLinkedAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /** @return 当前骨骼网格体上的 BBB 主动画实例 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    UBBBAnimInstance *GetBBBMainAnimInstanceThreadSafe() const;
};
