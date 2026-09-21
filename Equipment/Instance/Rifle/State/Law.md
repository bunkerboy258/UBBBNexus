# Rifle State Law

- 本状态的构造与写权限同时受 [`../../../../DomainData/Law.md`](../../../../DomainData/Law.md) 约束。

- 本目录只保存步枪跨帧事实，`FBBBRifleState` 是当前步枪唯一行为状态。
- 状态只能由 `FBBBRifleSignature::Initialize/Resolve/Restore` 写入，其它对象只能读取。
- 状态不得保存 UObject 引用、角色引用、网络组件、动态输入队列或临时解析上下文。
- 只在后续帧仍有语义的数据允许进入 State，单次输入和局部计算不得驻留。
