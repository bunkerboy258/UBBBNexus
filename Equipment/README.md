# Equipment

`ABBBEquipment` 只负责演员、网格、生命周期和角色进入装备领域的单一命令边界。

`Base/` 保存装备共享演员与配置基类，`Template/` 提供全装备固定输入和静态运行时。每种装备在 `Instance/<Weapon>/` 只定义配置、状态以及公共输入的 `Resolve/Restore` 实现。

角色只提交 `FBBBEquipmentCommand`。具体武器在自己的 `SubmitCommand` 中映射为静态输入包，之后不再有通用队列、Fragment、System 链或运行时行为分派。

换弹通知位于 `Instance/Rifle/Notify/`，仅提交步枪输入。旧装备配置与蓝图不迁移，后续按 `UBBBRifleDefinition` 和 `ABBBRifleEquipment` 重建。
