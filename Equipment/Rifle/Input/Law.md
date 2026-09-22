# Rifle Input Law

- 本目录只组织步枪行为输入
- 每种输入的声明与实现放在 `Packet/` 对应的一组 h 与 cpp
- 跨帧输入存储归 DomainData 下的 InputState 禁止恢复 InputFrame 或独立 PendingInputs 成员
- 调用上下文归 DomainData 下的 Context 目录 禁止把行为方法搬回 Context
- 外部通过装备根的固定 Submit 入口提交 包不能由外部直接执行
- 与角色输入共享 IsValid CanApply Apply 的规则表达方式 不强行共用角色管线
