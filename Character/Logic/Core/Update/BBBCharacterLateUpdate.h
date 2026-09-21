#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "BBBCharacterLateUpdate.generated.h"

class FBBBCharacterUpdatePipeline;

/**
 * 在角色移动组件完成本帧移动后驱动角色LateUpdate
 */
USTRUCT()
struct ABBB_EVAC_API FBBBCharacterLateUpdate final : public FActorTickFunction
{
    GENERATED_BODY()

    /**
     * 执行角色移动后的更新阶段
     * @param DeltaTime                当前帧间隔
     * @param TickType                 当前帧更新类型
     * @param CurrentThread            当前执行线程
     * @param MyCompletionGraphEvent   当前更新完成事件
     */
    virtual void ExecuteTick(
        float DeltaTime,
        ELevelTick TickType,
        ENamedThreads::Type CurrentThread,
        const FGraphEventRef &MyCompletionGraphEvent) override;

private:
    friend class FBBBCharacterUpdatePipeline;

    /** CMC 后更新所属管线 */
    FBBBCharacterUpdatePipeline *Pipeline = nullptr;
};

template<>
struct TStructOpsTypeTraits<FBBBCharacterLateUpdate>
    : public TStructOpsTypeTraitsBase2<FBBBCharacterLateUpdate>
{
    enum
    {
        WithCopy = false
    };
};
