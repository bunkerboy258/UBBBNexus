# Character Equipment Controller Law

- 本控制器只管理角色库存、装备选择、装备演员生命周期以及角色命令到装备动态边界的转发。
- 角色侧禁止实现具体武器规则、弹药判断、射击结果或换弹阶段结果。
- 每次调用装备边界必须显式传入 `bIsMirror`；Causal 命令与 Mirror 事实不得混用。
- Mirror 只允许把网络确认事实投递给当前装备；Causal 只允许提交本机控制命令。
- 装备切换必须先收束旧实例，再附着新实例，生命周期操作只能由 Lifecycle Processor 执行。
