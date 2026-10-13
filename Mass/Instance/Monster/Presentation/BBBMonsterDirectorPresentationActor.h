#pragma once
#include "GameFramework/Actor.h"
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterDirectorReplicationState.h"
#include "BBBMonsterDirectorPresentationActor.generated.h"

class UBBBMonsterDirectorDefinition;
class UBBBMonsterDirectorNetworkComponent;
class UMassEntityConfigAsset;
class ABBBMonsterSpawnRegionPresentationActor;

/** 地图配置与操作桥接 导演玩法由 Mass 维护 */
UCLASS(Blueprintable, meta = (DisplayName = "副本小怪导演"))
class ABBB_EVAC_API ABBBMonsterDirectorPresentationActor : public AActor
{
    GENERATED_BODY()
public:
    /** @return 创建无玩法 Tick 的地图入口 */
    ABBBMonsterDirectorPresentationActor();

    /** 当前副本的阶段与生成规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "副本导演", meta = (DisplayName = "导演定义"))
    TObjectPtr<UBBBMonsterDirectorDefinition> Definition;

    /** 不包含具体小怪行动片段的导演模板 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "副本导演", meta = (DisplayName = "导演实体模板"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;

    /** 当前地图明确绑定的生成区域 */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "副本导演", meta = (DisplayName = "生成区域"))
    TArray<TObjectPtr<ABBBMonsterSpawnRegionPresentationActor>> Regions;

    /** 在 BeginPlay 提交开始输入 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "副本导演", meta = (DisplayName = "运行时自动开始"))
    bool bAutoStart = false;

    /** 编辑器预览的阶段索引 不参与运行逻辑 */
    UPROPERTY(EditAnywhere, Category = "出生预览", meta = (DisplayName = "预览阶段", ClampMin = "0"))
    int32 PreviewStageIndex = 0;

    /**
     * @param Error	无效配置的原因
     * @return 当前地图绑定是否完整
     */
    bool ValidateConfiguration(FString& Error) const;

    /** @return 当前导演的只读显示结果 */
    UFUNCTION(BlueprintPure, Category = "副本导演", meta = (DisplayName = "读取导演状态"))
    FBBBMonsterDirectorReplicationState ReadState() const;

    /** @return 是否成功提交开始输入 */
    UFUNCTION(BlueprintCallable, Category = "副本导演", meta = (DisplayName = "开始导演"))
    bool StartDirector();
    /** @return 是否成功提交暂停输入 */
    UFUNCTION(BlueprintCallable, Category = "副本导演", meta = (DisplayName = "暂停导演"))
    bool PauseDirector();
    /** @return 是否成功提交继续输入 */
    UFUNCTION(BlueprintCallable, Category = "副本导演", meta = (DisplayName = "继续导演"))
    bool ResumeDirector();
    /** @return 是否成功提交停止输入 */
    UFUNCTION(BlueprintCallable, Category = "副本导演", meta = (DisplayName = "停止导演"))
    bool StopDirector();
    /** @return 是否成功触发当前等待阶段 */
    UFUNCTION(BlueprintCallable, Category = "副本导演", meta = (DisplayName = "触发当前阶段"))
    bool TriggerStage();

    /**
     * @param StageIndex	指定阶段
     * @param Result	区域纯值配置
     * @return 是否取得完整区域
     */
    bool ReadPlacementRegions(int32 StageIndex, TArray<FBBBMonsterSpawnRegionDefinition>& Result) const;

    /** @return 导演网络桥接组件 */
    UBBBMonsterDirectorNetworkComponent* GetDirectorNetwork() const;

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
    /** 当前结果的 UE 复制桥接 */
    UPROPERTY(VisibleAnywhere, Category = "副本导演", meta = (DisplayName = "导演网络组件"))
    TObjectPtr<UBBBMonsterDirectorNetworkComponent> DirectorNetwork;
    /** @return 在游戏世界安全创建当前入口所属实体 */
    bool EnsureEntity();
};
