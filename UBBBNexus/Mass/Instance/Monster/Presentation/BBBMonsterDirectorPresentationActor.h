#pragma once
#include "GameFramework/Actor.h"
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorStatus.h"
#include "BBBMonsterDirectorPresentationActor.generated.h"

class UBBBMonsterDirectorDefinition;
class APlayerState;
class UMassEntityConfigAsset;
class ABBBMonsterSpawnRegionPresentationActor;

/** 地图导演配置与操作桥接 调度玩法由 Mass 仅在服务端维护 */
UCLASS(Blueprintable, meta = (DisplayName = "副本小怪导演"))
class ABBB_EVAC_API ABBBMonsterDirectorPresentationActor : public AActor
{
    GENERATED_BODY()
public:
    ABBBMonsterDirectorPresentationActor();

    /** 关卡导演静态规则 包含固有僵尸 动态波次与预制波次 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (DisplayName = "导演定义"))
    TObjectPtr<UBBBMonsterDirectorDefinition> Definition;

    /** 仅装配导演 Trait 的 Mass 模板 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (DisplayName = "导演实体模板"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;

    /** 当前地图绑定的全部生成区域 包含固有区 预制波次区与禁刷区 */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Director", meta = (DisplayName = "生成区域列表"))
    TArray<TObjectPtr<ABBBMonsterSpawnRegionPresentationActor>> Regions;

    /** 在 BeginPlay 自动启动导演 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (DisplayName = "运行时自动开始"))
    bool bAutoStart = true;

    /**
     * @param Error	无效配置的原因
     * @return 当前地图绑定是否完整
     */
    bool ValidateConfiguration(FString& Error) const;

    /** @return 当前导演的只读运行状态 */
    UFUNCTION(BlueprintPure, Category = "Director", meta = (DisplayName = "读取导演状态"))
    FBBBMonsterDirectorStatus ReadStatus() const;

    /** @return 是否成功提交开始输入 */
    UFUNCTION(BlueprintCallable, Category = "Director", meta = (DisplayName = "开始导演"))
    bool StartDirector();

    /** @return 是否成功提交停止输入 */
    UFUNCTION(BlueprintCallable, Category = "Director", meta = (DisplayName = "停止导演"))
    bool StopDirector();

    /**
     * @param bPaused	是否暂停周期性动态波次
     * @return 是否成功提交设置输入
     */
    UFUNCTION(BlueprintCallable, Category = "Director", meta = (DisplayName = "暂停或恢复动态波次"))
    bool SetDynamicHordePaused(bool bPaused);

    /**
     * @param EventId	配置中的预制波次标识
     * @param InitialTarget	可选的指定初始目标玩家 为空则自动锁定最近玩家
     * @return 是否成功提交触发输入
     */
    UFUNCTION(BlueprintCallable, Category = "Director", meta = (DisplayName = "触发预制波次"))
    bool TriggerEventWave(FName EventId, APlayerState* InitialTarget = nullptr);

    /**
     * @param EventId	要停止的预制波次标识 None 表示停止全部
     * @return 是否成功提交停止输入
     */
    UFUNCTION(BlueprintCallable, Category = "Director", meta = (DisplayName = "停止预制波次"))
    bool StopEventWave(FName EventId = NAME_None);

    /**
     * @param Result	采集到的禁刷区域纯值快照
     * @return 无返回值
     */
    void ReadExcludeRegions(TArray<FBBBMonsterSpawnRegionDefinition>& Result) const;

    /**
     * @param RegionIds	要查找的区域标识列表
     * @param Result	匹配的可刷区域纯值快照
     * @return 是否找到至少一个有效区域
     */
    bool ReadRegionsByIds(const TArray<FName>& RegionIds, TArray<FBBBMonsterSpawnRegionDefinition>& Result) const;

    /**
     * @param Packet	投给当前导演实体的输入
     * @return 是否成功投递
     */
    template<typename TPacket>
    bool SubmitInput(TPacket&& Packet)
    {
        return EnsureEntity() && GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, Forward<TPacket>(Packet));
    }
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    /** 只保存所属实体句柄 不持有导演玩法状态 */
    FMassEntityHandle Entity;
    /** @return 在服务端游戏世界安全创建当前入口所属实体 */
    bool EnsureEntity();
};
