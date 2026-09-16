# 装备 API 与动画驱动换弹

角色仅向 `FBBBEquipmentExternalAPI` 提交 Equip、Fire、Reload、DetachMagazine、LoadMagazine、CancelReload 请求。API 入队不表示请求已经成功；装备内部仲裁和执行成功后，才通过角色 ExternalAPI 发布事件与后坐力。

## 更新顺序

1. 角色主更新：首次创建出生装备，处理输入／意图／请求／仲裁／执行；角色装备系统每帧只更新一次，维护实例关系并向装备 API 提交命令
2. 角色移动与 LateUpdate：发布角色动画事实、处理蒙太奇队列
3. 角色骨骼动画更新完成
4. 装备 Actor：`TG_PostUpdateWork`，依赖持有角色 Actor 与角色骨骼网格 Tick，自主执行一次装备主管线更新，内部依次处理命令与发布动画快照
5. 装备骨骼网格：`TG_PostUpdateWork`，依赖装备 Actor Tick，读取刚发布的快照

装备执行结果进入角色 API 时同步角色的换弹身份，供下一帧仲裁读取；事件包保留至下一轮角色网络上传和帧末清理。角色动画下一帧读取装备快照。角色帧末清理不会清除尚未消费的动画通知队列。

## 蒙太奇资产配置

在装备配置指定的换弹蒙太奇上添加两个 **Montage Notify**，使用标准 Play Montage Notify 类型：

- 卸下弹夹关键帧：Notify Name 为 `BBB.Reload.Start`
- 装填弹夹关键帧：Notify Name 为 `BBB.Reload.End`

换弹蒙太奇由角色动画系统的 `UBBBReloadMontagePlayback` 播放，并按蒙太奇实例 ID 关联不可变的操作序号。通知自动转交角色的两个蓝图可调用入口：

- `ReportReloadStartNotify(Sequence)`
- `ReportReloadEndNotify(Sequence, EndReason)`

普通装备／开火蒙太奇继续使用 `ExecuteEquipmentActionMontage`。动画图仍需包含换弹蒙太奇使用的 Slot；蒙太奇期间的 IK 遮蔽由动画图或曲线实现。

动画通知进入角色 Input → Intent → Request → Arbitration → Execution，之后才进入装备 API 的命令队列。角色验证当前换弹身份，装备验证阶段顺序和弹药条件。

角色在装备接受换弹后禁止 Fire 和重复 Reload；仍允许切换装备。切换会取消当前换弹。旧序号、重复关键帧不会修改新一轮换弹。

播放中断或失败会取消；若自然播放完成却没有结束通知，报警并取消，不自动补弹。取消保留已执行的弹药结果：卸下后仍为空，装填后保持已装弹量。

## 弹药与配置

`UBBBEquipmentDefinition` 仍由实例引用，目录接受该类型及其派生配置对象。新增 `AmmoConfig.AmmoCapacity`，默认 30；初始化装满。成功开火扣 1，卸下清空，装填补满当前配置容量。当前没有备用弹药、实体弹匣或拾取逻辑。

装备内部仲裁处理弹药不足、满弹换弹、开火间隔、重复换弹及阶段乱序。角色装备处理器不读取弹药来裁决。

## 镜像

动作包携带角色动作、操作序号、已执行阶段与执行后弹量，不再携带时长。角色网络恢复把事实交给装备 API 的 `ApplySnapshot`；镜像直接接受事实，不运行本地玩法仲裁。模拟角色的动画通知不产生本地输入。

## 迁移与验证

已删除装备动作通用 Result、装备／换弹计时参数、计时动画 getter、`TryGetWeaponReloading` 和瞄准 IK 的计时锁权重。使用这些节点的蓝图需要移除旧引用。新增动画事实 getter：`GetLoadedAmmo`、`GetAmmoCapacity`。

自动化测试筛选：`BBB.Equipment`。覆盖弹药／换弹仲裁、阶段执行与取消，以及装备和网格的 Tick 分组。
