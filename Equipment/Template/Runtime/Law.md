# Equipment Template Runtime Law

- Runtime 的状态持有、Context 和依赖生命周期同时受 [`../../../Constitution/Data/Law.md`](../../../Constitution/Data/Law.md) 约束。

- `TBBBEquipmentRuntime` 是全部装备固定输入顺序的无状态调度器。
- 调度器只依据槽位 `bIsMirror` 静态调用签名的 `Resolve` 或 `Restore`，禁止运行时类型查找和虚函数分派。
- `TBBBEquipmentRuntimeData` 是装备实例公开持有的唯一 C，只能公开保存具体装备声明的 B。
- 具体装备的 `DomainState` 私有持有行为状态与公共固定输入状态，并只向外公开 `ReadA` 只读方法。
- `FBBBEquipmentRuntimeContext` 必须位于 `Context/`，只保存本次解析需要的公开字段，禁止跨帧持有或保存执行身份。
- 每次 `Update` 结束必须清空全部固定输入激活标记。
