# 装备 API 与动画驱动换弹

`ABBBEquipment` 位于 `Equipment/` 根目录；装备实例的 Core、ExternalAPI、Pipeline、Runtime 和 System 位于 `Equipment/Instance/`。`Catalog/` 保留在根目录。角色采用同样的 `Character/BBBCharacter` 与 `Character/Instance/` 结构。

角色向 `FBBBEquipmentExternalAPI` 提交 Equip、Fire、Reload、DetachMagazine、LoadMagazine、CancelReload 请求，内部统一写入有序 Input 队列。API 入队不表示请求已经成功；系统 Processor 完成裁决和执行后，才通过角色 ExternalAPI 发布事件与后坐力。`SubmitCancelPendingActions()` 取消它之前尚未执行的操作，并取消未完成的换弹。

## 更新顺序

1. 角色主更新：处理输入／意图／请求／仲裁／执行；角色装备系统每帧只更新一次，维护实例关系并向装备 API 提交命令
2. 角色移动与 LateUpdate：发布角色动画事实、处理蒙太奇队列
3. 角色骨骼动画更新完成
4. 装备 Actor：`TG_PostUpdateWork`，依赖持有角色 Actor 与角色骨骼网格 Tick，自主依次更新 Input、Equip、Reload、Fire、Animation，最后清理已消费的系统输入；新到达的外部输入保留至下一帧
5. 装备骨骼网格：`TG_PostUpdateWork`，依赖装备 Actor Tick，读取刚发布的快照

装备执行结果进入角色 API 时同步角色的换弹身份，供下一帧仲裁读取；事件包保留至下一轮角色网络上传和帧末清理。角色动画下一帧读取装备快照。角色帧末清理不会清除尚未消费的动画通知队列。

角色出生时只建立空的背包槽和快捷栏，不自动创建或装备武器。当前本地装备来源尚未接入；模拟角色仍可通过装备恢复包创建镜像实例。`FBBBCharacterEquipmentLifecycleProcessor` 管理创建、挂接、显隐、动画绑定、解除和销毁；角色结束时由 `FBBBCharacterShutdown` 发起清理。装备自身没有激活标记或激活、解除、销毁包装接口。收起的持有装备继续自主更新，消费普通取消输入。

同帧按系统顺序处理：换弹开始成功后禁止开火；装填完成后允许使用新弹量开火；卸弹后按空弹处理。各系统内部保留输入顺序。主管线只调度，不承载具体行为分支。

## 系统与黑板

系统和黑板均为固定成员，不动态装配。Input 解析和转发外部输入；各 System 提供内部初始化、更新入口，并调用自己的 Processor。Fragment 位于所属系统的 `Fragment/` 下，仅负责可配置行为，不持有逐装备运行状态。

系统数据定义位于各自的 `Definition/` 下，通过私有成员、前向声明和精确友元限制访问。Fire 持有弹量、容量、开火时间和开火序号；Reload 通过注入的 Fire 数据完成卸弹和装填；Animation 读取最终事实，计算握持目标与瞄准来源并一次性发布只读快照。

## 蒙太奇资产配置

在装备配置的 `ReloadFragment` 指定的换弹蒙太奇上添加两个 **Montage Notify**，使用标准 Play Montage Notify 类型：

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

`UBBBEquipmentDefinition` 仍由实例引用，目录接受该类型及其派生配置对象。装备、开火、换弹分别由 `EquipFragment`、`FireFragment`、`ReloadFragment` 选择实现并保存配置；默认实现为标准挂接配置、单投射物开火和弹夹换弹。`FireFragment.AmmoCapacity` 默认 30，初始化装满。成功开火扣 1，卸下清空，装填补满当前配置容量。当前没有备用弹药、实体弹匣或拾取逻辑。

装备内部仲裁处理弹药不足、满弹换弹、开火间隔、重复换弹及阶段乱序。角色装备处理器不读取弹药来裁决。

## 镜像

动作包携带角色动作、操作序号、已执行阶段与执行后弹量，不再携带时长。角色网络恢复把事实交给装备 API 的 `ApplySnapshot`，统一进入 Input。镜像按收到顺序直接还原事实，系统只消费相应表现，不运行本地玩法仲裁。模拟角色的动画通知不产生本地输入。

## 迁移与验证

已删除装备动作通用 Result、装备／换弹计时参数、计时动画 getter、`TryGetWeaponReloading` 和瞄准 IK 的计时锁权重。使用这些节点的蓝图需要移除旧引用。新增动画事实 getter：`GetLoadedAmmo`、`GetAmmoCapacity`。

`BBBEquipmentReloadTests.cpp` 已按要求删除。代码编译不代表编辑器内配置已经迁移；旧配置字段不会自动写入三个片段，需在装备数据资产中设置片段类型并核对原值。

本轮删除顶层 `AmmoConfig`，容量需改在 `FireFragment.AmmoCapacity` 配置。删除装备动画实例的 `HasMainHandEquipment` 和对应快照字段，主手持有状态应读取角色侧事实。Fragment 反射类型名保持不变，源码路径迁入系统内部。资产修改由用户完成。
