# Character Law

- 角色 Actor 同时受 [Actor 宪法](../Constitution/Actor/Law.md) 约束。
- 所有角色领域数据同时受 [数据宪法](../Constitution/Data/Law.md) 约束。
- ABBBCharacter 是角色实例的唯一根载体 具体逻辑必须下沉。
- 通用角色输入只能通过 ABBBCharacter 的 SubmitInput 方法提交。
- ItemSystem 维护真实物品 背包与穿戴物品位置 物品栏选择及目标主手结果。
- AppearanceSystem 只读物品位置 独占角色外观语义与既成显示结果。
- EquipmentSystem 维护实际装备关系及镜像装备实例。
- 系统之间通过所属领域的公开只读状态衔接 禁止外部直接修改黑板。
- 角色代码禁止依赖具体外部实例类型 禁止拥有具体装备的实现逻辑和语义。
- 禁止在 Character/ 根目录新增直属文件夹或文件。
