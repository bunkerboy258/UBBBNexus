#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInput.h"

class FBBBEquipmentInitializer;
struct FBBBEquipmentInputRuntimeData;
struct FBBBEquipmentActionEvent;

/** 角色提交装备操作的唯一命令入口 */
class ABBB_EVAC_API FBBBEquipmentExternalAPI final
{
public:
    /**
     * 提交装备表现
     * @param Sequence	角色操作序号
     * @return 无
     */
    void SubmitEquip(int32 Sequence);

    /**
     * 提交开火
     * @param Sequence	角色操作序号
     * @return 无
     */
    void SubmitFire(int32 Sequence);

    /**
     * 提交换弹
     * @param Sequence	角色操作序号
     * @return 无
     */
    void SubmitReload(int32 Sequence);

    /**
     * 提交卸下弹夹
     * @param Sequence	换弹操作序号
     * @return 无
     */
    void SubmitDetachMagazine(int32 Sequence);

    /**
     * 提交装填弹夹
     * @param Sequence	换弹操作序号
     * @return 无
     */
    void SubmitLoadMagazine(int32 Sequence);

    /**
     * 取消未结束的换弹
     * @param Sequence	换弹操作序号
     * @return 无
     */
    void SubmitCancelReload(int32 Sequence);

    /**
     * 提交镜像事实，不经过本地玩法仲裁
     * @param Snapshot	角色传入的已执行事实
     * @return 无
     */
    void ApplySnapshot(const FBBBEquipmentActionEvent &Snapshot);

    /**
     * 取消此前尚未处理的操作及未结束的换弹
     * @return 无
     */
    void SubmitCancelPendingActions();

private:
    friend class FBBBEquipmentInitializer;

    /** 绑定外部输入接收区 */
    void Initialize(FBBBEquipmentInputRuntimeData &InInput, bool bInIsMirror);

    void Enqueue(EBBBEquipmentInputType Type, int32 Sequence);

    FBBBEquipmentInputRuntimeData *Input = nullptr;
    bool bIsMirror = false;
};
