# Aim Controller Law

- 本控制器只把解析后的控制事实转换为本机瞄准状态和瞄准目标。
- 只有 `Causal` 角色允许执行 `Update`；Mirror 角色只接受网络还原后的最终瞄准状态。
- Processor 必须先在局部状态上完成计算，再通过 `FBBBAimRuntimeData` 一次性提交。
- 禁止在本领域发送网络数据、播放动画、修改移动组件或直接处理玩家输入设备。
