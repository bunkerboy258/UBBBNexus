#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Appearance/Data/BBBAppearanceSelection.h"
#include "BBBWork/UBBBNexus/Appearance/Data/BBBAppearanceItem.h"
#include "BBBAppearanceComponent.generated.h"

/** 独立外观入口 蓝图负责组装 此组件只维护选择与同步 */
UCLASS(Blueprintable, ClassGroup = "BBB", meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UBBBAppearanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** 创建无逐帧更新的外观组件 */
    UBBBAppearanceComponent();

    /**
     * 建立初始外观
     * @return 无
     */
    virtual void BeginPlay() override;

    /**
     * 注册完整组合的属性复制
     * @param OutLifetimeProps	复制属性列表
     * @return 无
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    /**
     * 应用本机已经完成的外观选择并同步
     * @param Selection	完整选择
     * @return 是否应用成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Appearance")
    bool CommitAppearance(const FBBBAppearanceSelection &Selection);

    /**
     * 只更新本地展示对象 不修改正式选择或发送网络消息
     * @param Selection	预览组合
     * @return 是否显示成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Appearance")
    bool PreviewAppearance(const FBBBAppearanceSelection &Selection);

    /** @return 当前完整外观组合 */
    UFUNCTION(BlueprintPure, Category = "BBB|Appearance")
    FBBBAppearanceSelection GetAppearance() const;

    /**
     * 读取配置表条目
     * @param ItemId	稳定行名
     * @param Item	找到的资源配置
     * @return 条目是否存在
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Appearance")
    bool GetItem(FName ItemId, FBBBAppearanceItem &Item) const;

    /**
     * 加载条目资源供蓝图组装 不修改目标组件
     * @param ItemId	目录行名
     * @param Mesh	模型资源 可选空部件返回空
     * @param Materials	模型原始材质 用于替换旧部件的材质覆盖
     * @return 是否成功解析并加载
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Appearance")
    bool LoadItem(FName ItemId, USkeletalMesh *&Mesh, TArray<UMaterialInterface *> &Materials) const;

    /**
     * 本地检查完整组合并调整裤靴搭配 不用于网络接收
     * @param Selection	待检查并调整的组合
     * @return 是否可用
     */
    bool PrepareSelection(FBBBAppearanceSelection &Selection) const;

    /**
     * 读取指定部位按行名排序的可选条目
     * @param Slot	部位名称
     * @return 条目行名
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Appearance")
    TArray<FName> GetItems(FName Slot) const;

    /**
     * 由具体人物蓝图实现网格材质和附件组装
     * @param Selection	需要显示的完整组合
     * @return 是否完成组装
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "BBB|Appearance")
    bool ApplyAppearance(const FBBBAppearanceSelection &Selection);

    /** 该人物允许使用的外观资源 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Appearance")
    TObjectPtr<UDataTable> Catalog;

    /** 具体人物蓝图配置的默认组合 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Appearance")
    FBBBAppearanceSelection DefaultSelection;

private:
    /**
     * 接收拥有者已经形成的结果 只检查结构
     * @param Selection	客户端最终组合
     * @return 无
     */
    UFUNCTION(Server, Reliable)
    void ServerCommitAppearance(FBBBAppearanceSelection Selection);

    /** 收到最终组合后重建本地表现 */
    UFUNCTION()
    void OnRep_Selection();

    /** 当前生效的完整组合 用于后加入和重新相关时恢复 */
    UPROPERTY(ReplicatedUsing = OnRep_Selection)
    FBBBAppearanceSelection CurrentSelection;
};
