#pragma once
#include "GameFramework/Actor.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnerPresentationActor.generated.h"

class UBBBMonsterSpawnDefinition;
class UMassEntityConfigAsset;

/** 地图中的统一生成入口 不承担逐帧玩法 */
UCLASS(Blueprintable, meta = (DisplayName = "小怪生成器"))
class ABBB_EVAC_API ABBBMonsterSpawnerPresentationActor final : public AActor
{
    GENERATED_BODY()
public:
    /** @return 创建无玩法 Tick 的地图桥接 无返回值 */
    ABBBMonsterSpawnerPresentationActor();

    /** 模板组合 选点限制 朝向与随机规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成器", meta = (DisplayName = "出生配置"))
    TObjectPtr<UBBBMonsterSpawnDefinition> Definition;

    /** 只装配生成请求的 Mass 模板 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成器", meta = (DisplayName = "生成入口模板"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;

    /** 相对当前生成器变换的局部区域 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成器", meta = (DisplayName = "生成区域"))
    FBBBMonsterSpawnRegionDefinition Region;

    /** 每次请求的有限数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成器", meta = (DisplayName = "生成数量", ClampMin = "1", ClampMax = "512"))
    int32 Count = 20;

    /** 是否在游戏开始时提交一次请求 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成器", meta = (DisplayName = "自动开始"))
    bool bAutoStart = false;

    /** 非零时覆盖出生配置的种子 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成器", meta = (DisplayName = "种子覆盖"))
    int32 SeedOverride = 0;

    /** @return 已有请求是否已完成且新请求成功投递 */
    UFUNCTION(BlueprintCallable, Category = "生成器", meta = (DisplayName = "开始生成"))
    bool StartSpawning();

    /** @return 是否投递暂停请求 */
    UFUNCTION(BlueprintCallable, Category = "生成器", meta = (DisplayName = "暂停生成"))
    bool PauseSpawning();

    /** @return 是否投递继续请求 */
    UFUNCTION(BlueprintCallable, Category = "生成器", meta = (DisplayName = "继续生成"))
    bool ResumeSpawning();

    /** @return 是否投递停止请求 已生成实体不受影响 */
    UFUNCTION(BlueprintCallable, Category = "生成器", meta = (DisplayName = "停止生成"))
    bool StopSpawning();

    /** @return 当前请求实际创建的数量 */
    UFUNCTION(BlueprintPure, Category = "生成器", meta = (DisplayName = "已生成数量"))
    int32 ReadCreatedCount() const;

    /** @return 当前出生区域的世界空间值 */
    FBBBMonsterSpawnRegionDefinition ReadRegion() const;

    /** @return 当前请求的诊断说明 */
    UFUNCTION(BlueprintPure, Category = "生成器", meta = (DisplayName = "生成状态"))
    FString ReadStatus() const;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
private:
    /** 只保存所属实体句柄 状态由 Mass 唯一维护 */
    FMassEntityHandle Entity;
    /** @return 游戏世界内是否已建立请求实体 */
    bool EnsureEntity();
};
