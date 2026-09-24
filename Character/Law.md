# Character Law

- 所有角色领域数据同时受 [`../Constitution/Data/Law.md`](../Constitution/Data/Law.md) 约束。

- `ABBBCharacter`是角色实例的唯一根载体,具体的逻辑必须下沉
- 通用角色输入只能通过 SubmitInput 提交 装备持有关系由 EquipmentSystem 的选择入口维护 禁止外部直接修改黑板
- 角色代码禁止依赖具体外部实例类型
- 输入不得包含装备选择 武器状态及动作事实
- 角色不得转发开火 换弹 弹药及武器网络消息
