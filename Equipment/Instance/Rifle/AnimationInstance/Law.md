# Rifle Animation Instance Law

- 只保存动画图实际使用的步枪动作快照
- 由 Animation 系统统一发布 动画图与线程安全 getter 只读
- 角色动画实例不转发换弹与开火语义
