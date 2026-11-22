#pragma once

#include "InstanceDataTypes.h"

class UInstancedSkinnedMeshComponent;

/** 一个可视实例及其 GPU 动画轨道 */
struct FBBBMonsterCrowdInstance final
{
    TWeakObjectPtr<UInstancedSkinnedMeshComponent> Component;
    FPrimitiveInstanceId Id;
    int32 Track = INDEX_NONE;
    int32 Sequence = INDEX_NONE;
    float PositionSeconds = 0.0f;
    FTransform Display = FTransform::Identity;
    uint64 SeenFrame = 0;
};
