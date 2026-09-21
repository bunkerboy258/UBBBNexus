# Rifle Domain Data Law

- 本领域数据的构造与写权限同时受 [`../../../../Constitution/Data/Law.md`](../../../../Constitution/Data/Law.md) 约束。

- `States/` 只保存步枪跨帧事实，`FBBBRifleState` 是当前步枪唯一行为状态 A。
- `FBBBRifleDomainState` 是 `FBBBRifleState` 与公共输入状态的唯一直接持有者 B。
- 状态只能由 `FBBBRifleSignature::Initialize/Resolve/Restore` 写入，其它对象只能读取。
- 状态不得保存 UObject 引用、角色引用、网络组件、动态输入队列或临时解析上下文。
- 只在后续帧仍有语义的数据允许进入 State，单次输入和局部计算不得驻留。
