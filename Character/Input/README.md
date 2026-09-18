# 角色输入

外部只通过 `ABBBCharacter::GetInput()` 提交两个普通输入包

| 包 | 用途 | 生命周期 |
| --- | --- | --- |
| `FBBBCharacterContinuousInput` | 移动 朝向 瞄准和步态的持续事实 | 保留最后一份快照 |
| `FBBBCharacterDiscreteInput` | 开火 换弹 切换装备 动画通知 蒙太奇和相机表现 | 本帧消费一次 |

网络接收端使用 `FBBBCharacterRestoreDiscreteInput` 表示权威端已经形成的离散事实 它不是外部输入 网络组件将它提交到同一输入入口 解析系统依照镜像规则写入黑板

`FBBBCharacterInput` 只负责校验和收件 `ParseSystem` 中的 `FBBBCharacterInputProcessor` 在主管线的固定位置解析所有包 输入包只携带事实和修改效果 不持有规则对象 不注册委托 不直接调用任一控制器

普通离散输入的效果按固定顺序执行

1. 先接收装备已经形成的动作事实
2. 开火效果写入开火事实
3. 换弹效果清除开火事实并建立换弹阶段
4. 切换装备效果取消换弹并清除开火和换弹事实
5. 将最终控制事实提交给控制器和装备命令黑板

因此冲突不由隐式优先级或输入到达顺序决定 例如换弹打断开火 切换装备打断换弹 都能在这一个处理器中直接看见

## 手动资产迁移

1. 在玩家控制器的 `PlayerInputSystem` 配置 InputAction 引用
2. 保留 `FullBody` `UpperBody` `FullBodyAdditivePreAim` `UpperBodyAdditive` `AdditiveHitReact` 五个动画 Slot
3. 换弹通知继续使用 `BBB.Reload.Start` 和 `BBB.Reload.End` 并经 `ReportReloadStartNotify` 和 `ReportReloadEndNotify` 提交离散输入
4. 不要同时添加自动通知和手工通知

输入动作无效 槽位无效或蒙太奇播放失败会产生日志 镜像角色不会通过网络系统直接修改角色黑板
