#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBCharacterParseState.generated.h"

class ABBBEquipment;
class FBBBCharacterParseSystem;
class FBBBCharacterInputProcessor;

/** 包解析过程需要保留的跨帧操作事实 */
USTRUCT()
struct FBBBCharacterParseState
{
    GENERATED_BODY()

public:
    /** @return 当前是否还原模式 */
    bool IsRestoreMode() const
    {
        return bRestoreMode;
    }

    /** @return 是否存在进行中的换弹 */
    bool IsReloadInProgress() const
    {
        return ReloadSequence > 0;
    }

    /** @return 本帧是否存在待切换装备 */
    bool IsEquipmentSwitchPending() const
    {
        return SelectedEquipment != nullptr;
    }

    /** @return 本帧是否已承诺换弹 */
    bool HasReloadCommitted() const
    {
        return bReload;
    }

    /** @return 本帧是否已承诺开火 */
    bool HasFireCommitted() const
    {
        return bFire;
    }

    /** @return 当前换弹是否已卸下弹匣 */
    bool IsMagazineDetached() const
    {
        return bMagazineDetached;
    }

    /** @return 当前换弹是否已排队结束 */
    bool IsReloadEndQueued() const
    {
        return bEndQueued;
    }

    /** @return 序号是否属于当前进行中的换弹 */
    bool IsCurrentReloadSequence(const int32 Sequence) const
    {
        return ReloadSequence > 0 && Sequence == ReloadSequence;
    }

    /** @return 序号是否为刚取消的换弹 */
    bool IsCancelledReloadSequence(const int32 Sequence) const
    {
        return CancelReloadSequence > 0 && Sequence == CancelReloadSequence;
    }

    /**
     * 帧初重置暂态并守卫换弹装备
     * @param bInRestoreMode	本帧是否还原模式
     * @param ActiveEquipment	当前激活主手装备 可为空
     */
    void BeginFrame(bool bInRestoreMode, ABBBEquipment *ActiveEquipment);

    /** 登记本帧开火承诺 */
    void CommitFire()
    {
        bFire = true;
    }

    /** 登记本帧换弹承诺 */
    void CommitReload()
    {
        bReload = true;
    }

    /** 登记本帧跳跃意图 */
    void CommitJump()
    {
        Control.bJump = true;
    }

    /**
     * 基底移动快照覆盖控制暂态 移动向量钳制到单位长度
     * @param MoveWorld	期望移动向量
     * @param FacingWorld	期望朝向
     * @param bInWalk	是否步行
     * @param bInSprint	是否冲刺
     * @param bInCrouch	是否蹲伏
     */
    void ApplyMovementSnapshot(FVector MoveWorld, FRotator FacingWorld, bool bInWalk, bool bInSprint, bool bInCrouch)
    {
        Control.MoveWorld = MoveWorld.GetClampedToMaxSize(1.0f);
        Control.FacingWorld = FacingWorld;
        Control.bWalk = bInWalk;
        Control.bSprint = bInSprint;
        Control.bCrouch = bInCrouch;
    }

    /**
     * 基底瞄准快照覆盖控制暂态
     * @param AimTargetWorld	瞄准点世界坐标
     * @param bInAim	是否瞄准
     */
    void ApplyAimSnapshot(FVector AimTargetWorld, bool bInAim)
    {
        Control.AimTargetWorld = AimTargetWorld;
        Control.bAim = bInAim;
    }

    /**
     * 登记本帧期望切换的装备
     * @param Equipment	期望切换的装备实例
     */
    void SelectEquipment(ABBBEquipment &Equipment);

    /** 取消进行中的换弹并保留取消序号供蒙太奇守卫 */
    void CancelReload();

    /**
     * 换弹开始事实驱动追踪开启
     * @param Sequence	换弹序号
     * @param FactEquipmentId	事实所属装备标识
     * @param ActiveEquipment	当前激活主手装备 可为空
     * @param ActiveEquipmentId	当前激活装备标识
     */
    void TrackReloadStarted(int32 Sequence, FName FactEquipmentId, ABBBEquipment *ActiveEquipment, FName ActiveEquipmentId);

    /**
     * 弹匣卸下事实推进追踪进度
     * @param Sequence	换弹序号
     * @param FactEquipmentId	事实所属装备标识
     * @param ActiveEquipmentId	当前激活装备标识
     */
    void TrackMagazineDetached(int32 Sequence, FName FactEquipmentId, FName ActiveEquipmentId);

    /**
     * 换弹完结事实收尾追踪
     * @param Sequence	换弹序号
     * @param FactEquipmentId	事实所属装备标识
     * @param ActiveEquipmentId	当前激活装备标识
     * @param bCancelled	是否取消完结
     */
    void TrackReloadFinished(int32 Sequence, FName FactEquipmentId, FName ActiveEquipmentId, bool bCancelled);

    /**
     * 动画通知报告弹匣卸下
     * @param Sequence	换弹序号
     * @return 通知是否被接受
     */
    bool ReportMagazineDetached(int32 Sequence);

    /**
     * 动画通知报告弹匣装填
     * @param Sequence	换弹序号
     * @return 通知是否被接受
     */
    bool ReportMagazineLoaded(int32 Sequence);

    /**
     * 动画通知报告换弹中断
     * @param Sequence	换弹序号
     * @return 通知是否被接受
     */
    bool ReportReloadInterrupted(int32 Sequence);

private:
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterInputProcessor;

    UPROPERTY()
    TWeakObjectPtr<ABBBEquipment> ReloadEquipment;

    UPROPERTY()
    TObjectPtr<ABBBEquipment> SelectedEquipment = nullptr;

    int32 ReloadSequence = INDEX_NONE;
    int32 LastCompletedReloadSequence = INDEX_NONE;
    int32 CancelReloadSequence = INDEX_NONE;
    bool bMagazineDetached = false;
    bool bEndQueued = false;
    bool bRestoreMode = false;
    bool bFire = false;
    bool bReload = false;

    FBBBCharacterControlFacts Control;
};
