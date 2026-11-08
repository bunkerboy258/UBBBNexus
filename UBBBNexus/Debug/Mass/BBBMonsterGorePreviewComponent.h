#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBMonsterGorePreviewComponent.generated.h"

class UBBBMonsterGoreDefinition;

/** 固定靶仅注入当前表现结果 不实现第二套伤口或玩法生命 */
UCLASS(meta = (DisplayName = "统一血腥表现预览"))
class ABBB_EVAC_API UBBBMonsterGorePreviewComponent final : public UActorComponent
{
    GENERATED_BODY()
  public:
    UBBBMonsterGorePreviewComponent();

    /** 由固定靶版本选择与覆盖统一解析 不在此维护第二个选择入口 */
    UPROPERTY(Transient, VisibleAnywhere, Category = "血腥预览", meta = (DisplayName = "当前伤口与血液配置"))
    TObjectPtr<UBBBMonsterGoreDefinition> Definition;

    /** 固定周期恢复完整身体 零关闭 不因连续命中延迟 */
    UPROPERTY(EditAnywhere, Category = "血腥预览", meta = (DisplayName = "自动重置间隔", ClampMin = "0", Units = "s"))
    float ResetInterval = 3;

    /** 每个部位达到此命中数后预览断开 不计算真实伤害 */
    UPROPERTY(EditAnywhere, Category = "血腥预览", meta = (DisplayName = "断肢预览所需命中数", ClampMin = "1", ClampMax = "50"))
    int32 HitsToSever = 4;

    /** 不改变正式程序化受击参数 */
    UPROPERTY(EditAnywhere, Category = "血腥预览", meta = (DisplayName = "启用程序化受击"))
    bool bEnablePhysicalReaction = true;

    /** 可暂时只预览局部创口和喷血 */
    UPROPERTY(EditAnywhere, Category = "血腥预览", meta = (DisplayName = "允许断肢预览"))
    bool bAllowSevering = true;

    /** 只清除此靶产生的环境残留 */
    UPROPERTY(EditAnywhere, Category = "血腥预览", meta = (DisplayName = "重置时清理本靶血迹"))
    bool bClearResidueOnReset = true;

    /** @return 清除当前伤口与局部物理状态 无返回值 */
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "血腥预览", meta = (DisplayName = "立即重置僵尸"))
    void ResetTarget();

    /** @param Hit 当前标准点命中 @param Strength 只影响血效强度 @return 无返回值 */
    void ApplyHit(const FBBBMonsterHitReactionFragment &Hit, float Strength);

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  private:
    FBBBMonsterGoreFragment Result;
    FBBBMonsterHitReactionFragment CurrentHit;
    int32 RegionHits[6] = {};
    uint8 Destroyed = 0;
    double NextReset = 0;
    double NextBlood = 0;
    float AppliedInterval = -1;
    void Present();
    void ClearResidue();
};
