# Rifle Runtime Data Law

严格遵守 `Constitution/Data/Law.md`

- `FBBBRifleRuntimeData` 是步枪唯一的聚合黑板 C
- 步枪演员公开持有 RuntimeData
- C 只公开持有 DomainState 不允许直接持有弹药 布尔标记 输入槽或 Context
- C 不提供读取转发 清理 重置或行为函数
- 当前只需要一个步枪领域 禁止为字段数量或形式对称拆出无独立职责的领域
