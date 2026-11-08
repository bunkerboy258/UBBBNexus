#pragma once

#include "CoreMinimal.h"

class UPrimitiveComponent;
class AActor;

/** 当前可见血迹的回收与局部积累参数 不保存命中事件 */
struct FBBBMonsterBloodResidue final
{
    /** 当前表现来源 不保存命中过程 */
    TWeakObjectPtr<AActor> Source;
    /** 当前淡出时间 */
    float FadeDuration = 4;
    /** 当前接收表面 防止不同高度或不同组件的血迹混合 */
    TWeakObjectPtr<UPrimitiveComponent> Surface;
    FVector Normal = FVector::UpVector;
    double CreatedAt = 0.0;
    float Lifetime = 0.0f;
    float Coverage = 1.0f;
    uint8 Kind = 0;
};
