#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemAddLocalControlPacket.h"
#include "GameFramework/Actor.h"
#include "BBBItemDebugActor.generated.h"

class ABBBCharacter;

/** 通过角色输入入口分帧向背包添加目标角色目录中的物品的调试演员 */
UCLASS(Blueprintable, meta = (DisplayName = "物品调试器"))
class ABBB_EVAC_API ABBBItemDebugActor : public AActor
{
    GENERATED_BODY()

public:
    /** @return 构造等待本地角色就绪的调试演员 */
    ABBBItemDebugActor();

    //~ Begin AActor Interface
    /** @return 校验调试参数并开始等待 */
    virtual void BeginPlay() override;

    /**
     * 角色就绪后每帧提交一批入包请求
     * @param DeltaSeconds	本帧间隔秒数
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;
    //~ End AActor Interface

protected:
    /** 穿戴物超过背包容量时 按目录顺序选择发放页 从零开始 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|物品调试", meta = (ClampMin = "0", DisplayName = "配装页"))
    int32 WearablePage = 0;

    /** 指定审核配装时只发放并穿上这些穿戴物 */
    UPROPERTY(EditInstanceOnly, Category = "BBB|物品调试", meta = (DisplayName = "审核配装"))
    TArray<FName> ReviewWearables;

    /** 显式目标角色 留空时使用本地玩家角色 */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BBB|物品调试", meta = (DisplayName = "目标角色"))
    TObjectPtr<ABBBCharacter> TargetCharacter;

    /** 未指定目标时查找的本地玩家索引 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|物品调试", meta = (ClampMin = "0", DisplayName = "玩家索引"))
    int32 PlayerIndex = 0;

    /** 等待角色及其动画初始化的最长秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|物品调试", meta = (ClampMin = "0.1", DisplayName = "等待超时时间"))
    float WaitTimeout = 10.0f;

private:
    /**
     * 按目标角色目录校验全部物品并组装分帧提交的输入包
     * @param Character 本次发放的目标角色
     * @return 是否有可提交的有效输入包
     */
    bool PrepareItemBatches(const ABBBCharacter &Character);

    /** 发放完成后 通过常规移动输入应用审核配装 */
    void ApplyReviewWearables(ABBBCharacter &Character) const;

    /** 每帧的调试器发放预算 包含本帧可能创建的装备实例 */
    static constexpr int32 ItemsPerFrame = 16;

    /** 第一批提交前确定的角色 后续批次只发给同一实例 */
    TWeakObjectPtr<ABBBCharacter> BatchTarget;

    /** 已校验的输入包 按配置顺序提交 */
    TArray<FBBBCharacterItemAddLocalControlPacket> ItemBatches;

    /** 下一批待提交输入包的位置 */
    int32 NextBatchIndex = 0;
};
