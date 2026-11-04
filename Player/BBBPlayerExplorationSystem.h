#pragma once
#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerExplorationRuntimeData.h"
#include "BBBPlayerExplorationSystem.generated.h"
class APlayerController;

/** 随本地玩家保留的探索模型 不依赖界面和物品系统 */
UCLASS()
class ABBB_EVAC_API UBBBPlayerExplorationSystem final : public ULocalPlayerSubsystem
{
    GENERATED_BODY()
public:
    /** 本关卡需要发现的耄耋数量 */
    static constexpr int32 TargetCount = 10;

    /** @param Controller 当前本地控制器 @return 无 只由控制器主管线调用 */
    void Update(APlayerController &Controller);

    /** @return 已发现的不同编号数量 */
    int32 GetDiscoveredCount() const;

    /** @return 首次全部发现的实际时间 未完成为负值 */
    double GetCompletionTime() const;

    /** 与角色实例生命周期无关的唯一探索黑板 */
    UPROPERTY(Transient)
    FBBBPlayerExplorationRuntimeData RuntimeData;
};
