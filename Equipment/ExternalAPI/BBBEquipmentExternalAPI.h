#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Definition/BBBEquipmentCommand.h"

class ABBBEquipmentInstance;
struct FBBBEquipmentActionEvent;

/** 角色提交装备操作的唯一命令入口 */
class ABBB_EVAC_API FBBBEquipmentExternalAPI final
{
public:
    /**
     * 绑定装备实例
     * @param InInstance	装备实例
     * @return 无
     */
    void Initialize(ABBBEquipmentInstance &InInstance);

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

private:
    void Enqueue(EBBBEquipmentCommandType Type, int32 Sequence);
    ABBBEquipmentInstance *Instance = nullptr;
};
