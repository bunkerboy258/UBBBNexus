# Equipment Law

- 所有装备领域数据同时受 [`../Constitution/Data/Law.md`](../Constitution/Data/Law.md) 约束。

- `Base/` 下是所有装备实例的基类ACTOR、基类动画实例和基类配置。
- `Catalog/` 只负责装备标识到配置资产的表构造。
- `Template/` 是所有武器实例的模板。
- `ABBBEquipment` 只承载演员、网格、生命周期和唯一的动态命令边界，不保存武器行为状态。
- 具体武器的状态、输入包、解析和动画通知必须归属 `Instance/<Weapon>/`。
- 角色只能向 `ABBBEquipment::SubmitCommand` 提交通用命令并显式提供 `bIsMirror`；武器实现负责将其映射为公共固定输入包。
- 装备的 Causal 路径可以生成事实，Mirror 路径只能还原事实和表现，禁止重新产生因果结果。
