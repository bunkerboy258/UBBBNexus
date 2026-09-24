# Rifle Action System Law

- 系统根只按固定顺序调度本领域处理器
- 使用装备根在栈上构造的本次 Context 不持有 Context
- DomainData 只保存 B DomainData/States 只保存 A
- B 直接私有持有 A 并提供对应 const 读取方法
- 持久状态由显式 friend 处理器通过 RuntimeData 访问
