#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Mass/EntityHandle.h"
#include "BBBMassValidationLibrary.generated.h"

class UMassEntityConfigAsset;

/** 仅在 PIE 中创建并检查明确配置的测试实体 不实现具体实例玩法 */
UCLASS()
class ABBB_EVAC_API UBBBMassValidationLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * 复制测试模板为临时全骨骼表现配置 不修改正式资产
     * @param WorldContext		当前 PIE 世界
     * @param Source		正式实体模板
     * @return 临时配置 调用方须在测试期间持有引用
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|验证")
    static UMassEntityConfigAsset* CreateActorStressConfig(UObject* WorldContext, UMassEntityConfigAsset* Source);

    /**
     * 使用引擎模板和出生位置处理器创建测试实体
     * @param WorldContext		当前 PIE 世界
     * @param Configs		循环分配的实体模板
     * @param Count		测试数量 上限一千
     * @param Center		网格出生中心 厘米
     * @param Spacing		相邻实体距离 厘米
     * @return 实际创建的完整代际句柄 仅在当前世界有效
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|验证")
    static TArray<FMassEntityHandle> SpawnPopulation(UObject* WorldContext, const TArray<UMassEntityConfigAsset*>& Configs, int32 Count, FVector Center, float Spacing);

    /**
     * 读取当前世界测试实体位置 速度和对应预算网格
     * @param WorldContext		当前 PIE 世界
     * @param Entities		本工具创建的完整句柄
     * @return JSON 验证快照 不存在的实体明确计入失效数
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|验证")
    static FString InspectPopulation(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities);

    /**
     * 仅回收本轮有效测试句柄 不销毁其它实体
     * @param WorldContext		当前 PIE 世界
     * @param Entities		本轮完整代际句柄
     * @return 实际回收数量
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|验证")
    static int32 DestroyPopulation(UObject* WorldContext, const TArray<FMassEntityHandle>& Entities);
};
