# Equipment Template Runtime Law

- Runtime 的状态持有、Context 和依赖生命周期同时受 [`../../../DomainData/Law.md`](../../../DomainData/Law.md) 约束。

- `TBBBEquipmentRuntime` 是全部装备固定输入顺序的唯一调度器。
- 调度器只依据槽位 `bIsMirror` 静态调用签名的 `Resolve` 或 `Restore`，禁止运行时类型查找和虚函数分派。
- Runtime 只持有具体签名状态和公共固定输入状态，禁止持有 UObject、角色业务状态或网络组件。
- `FBBBEquipmentRuntimeContext` 只保存本次解析需要的引用和帧间隔，禁止跨帧持有或保存执行身份。
- 每次 `Update` 结束必须清空全部固定输入激活标记。
