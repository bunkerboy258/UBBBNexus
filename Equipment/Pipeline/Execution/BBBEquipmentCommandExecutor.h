#pragma once
class ABBBEquipmentInstance;
struct FBBBEquipmentCommand;
struct FBBBEquipmentActionEvent;

/** 执行已批准命令并提交装备事实 */
class FBBBEquipmentCommandExecutor final
{
public:
    /**
     * 执行本地命令
     * @param Instance	装备实例
     * @param Command	已批准命令
     * @return 无
     */
    static void Execute(ABBBEquipmentInstance &Instance, const FBBBEquipmentCommand &Command);

    /**
     * 应用镜像事实
     * @param Instance	装备实例
     * @param Snapshot	角色还原的事实
     * @return 无
     */
    static void ApplySnapshot(ABBBEquipmentInstance &Instance, const FBBBEquipmentActionEvent &Snapshot);
};
