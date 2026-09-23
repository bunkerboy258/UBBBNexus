#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBRifleEquipment.generated.h"

class UBBBRifleDefinition;

/** 直接持有步枪配置、输入和运行时事实的唯一装备演员 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBRifleEquipment : public ABBBEquipment
{
    GENERATED_BODY()

public:
    ABBBRifleEquipment();

    /** 步枪实例跨帧运行时事实 */
    FBBBRifleRuntimeData RuntimeData;

    /**
     * 提交装备动作输入
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitEquipInput(int32 Sequence, bool bInIsMirror) override;

    /**
     * 提交开火输入
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitPrimaryInput(int32 Sequence, bool bInIsMirror) override;

    /**
     * 提交副行为输入
     * @param bActive        副行为是否激活
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitSecondaryInput(bool bActive, int32 Sequence, bool bInIsMirror) override;

    /**
     * 提交换弹输入
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitReloadInput(int32 Sequence, bool bInIsMirror) override;

    /**
     * 提交当前动作中断输入
     * @param bInIsMirror	是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitInterruptActiveActionInput(bool bInIsMirror) override;

    /**
     * 提交镜像还原事实
     * @param Fact           已确认装备事实
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitRestoreFact(const FBBBEquipmentActionFact &Fact, bool bInIsMirror) override;

    /**
     * 提交弹匣脱离通知输入
     * @param Sequence       换弹序号，INDEX_NONE 表示当前换弹
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    void SubmitDetachMagazineInput(int32 Sequence, bool bInIsMirror);

    /**
     * 提交弹匣装入通知输入
     * @param Sequence       换弹序号，INDEX_NONE 表示当前换弹
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    void SubmitLoadMagazineInput(int32 Sequence, bool bInIsMirror);

    /**
     * 提交换弹结束通知输入
     * @param Sequence       换弹序号，INDEX_NONE 表示当前换弹
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    void SubmitInterruptReloadInput(int32 Sequence, bool bInIsMirror);

private:
    /**
     * 在装备动画工作完成后直接解析步枪输入并发布事实
     * @param DeltaSeconds    帧间隔
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;

    /**
     * 根据步枪配置初始化运行时事实
     * @param InDefinition    装备静态配置
     * @return 初始化是否成功
     */
    virtual bool InitializeRuntimeData() override;

    /** 清除已解析或因运行环境无效而丢弃的输入 */
    void ResetPendingInputs();
};
