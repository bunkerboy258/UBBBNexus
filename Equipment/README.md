# Equipment

`ABBBEquipment` 只负责演员、网格、生命周期和角色进入装备领域的单一命令边界。

`Core/` 提供固定输入槽位与静态运行时模板。每种武器在 `Instance/<Weapon>/` 定义自己的状态、输入包和签名解析函数；`FBBBRifleSignature::Parse` 是当前步枪唯一的行为入口。

角色只提交 `FBBBEquipmentCommand`。具体武器在自己的 `SubmitCommand` 中映射为静态输入包，之后不再有通用队列、Fragment、System 链或运行时行为分派。

换弹通知位于 `Instance/Rifle/Notify/`，仅提交步枪输入。旧装备配置与蓝图不迁移，后续按 `UBBBRifleDefinition` 和 `ABBBRifleEquipment` 重建。
