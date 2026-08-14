#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "BBBCharacterLateUpdateTickFunction.generated.h"

class ABBBCharacter;

/**
 * 在角色移动组件完成本帧移动后驱动角色LateUpdate
 */
USTRUCT()
struct ABBB_EVAC_API FBBBCharacterLateUpdateTickFunction final : public FTickFunction
{
    GENERATED_BODY()

    /** 执行LateUpdate的所属角色 */
    ABBBCharacter *Target = nullptr;

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

    /** @return 更新依赖图中的诊断描述 */
    virtual FString DiagnosticMessage() override;

    /**
     * 获取更新分析器使用的诊断名称
     * @param bDetailed 是否返回包含角色名称的详细名称
     * @return 更新分析器使用的诊断名称
     */
    virtual FName DiagnosticContext(bool bDetailed) override;
};

template<>
struct TStructOpsTypeTraits<FBBBCharacterLateUpdateTickFunction>
    : public TStructOpsTypeTraitsBase2<FBBBCharacterLateUpdateTickFunction>
{
    enum
    {
        WithCopy = false
    };
};
