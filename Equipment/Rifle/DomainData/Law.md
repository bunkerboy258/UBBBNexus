# Rifle Domain Data Law

严格遵守 `Constitution/Data/Law.md`

- 直属根只放 `FBBBRifleDomainState` 与本 Law
- `States/` 每个头文件只定义一个 State
- `Context/` 每个头文件定义调用期间的纯数据 Context
- DomainState 直接私有持有 ActionState 与 InputState 不得增加总 State 中间层
- ActionState 保存弹药 开火与换弹的跨帧事实
- InputState 保存下一次固定解析时机之前收到的输入 与动作事实有不同的清理时机
- DomainState 为每个 State 提供 Read 加完整类型语义的只读 getter
- 装备根负责初始化与待解析输入维护 行为输入包负责动作事实维护
- 写入者由 DomainState 显式 friend 授权 读取者不得获得可变状态引用
- State 构造权限仅授予所属 DomainState 禁止独立构造或复制状态快照代替持有链
- Context 只能由步枪根在栈上创建 只保存本次依赖与临时数据 禁止增加服务方法
