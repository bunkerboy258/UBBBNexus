# Equipment Law

- 所有装备领域数据同时受 [`../Constitution/Data/Law.md`](../Constitution/Data/Law.md) 约束。

- `Base/` 只保存全部装备共享的演员、动画实例和配置基类，禁止保存具体装备行为。
- `Catalog/` 只负责装备标识到配置资产的查找，禁止创建实例或执行装备行为。
- `Template/` 只保存公共固定输入、静态运行时和单帧上下文，禁止依赖具体装备。
- `ABBBEquipment` 只承载演员、网格、生命周期和唯一的动态命令边界，不保存武器行为状态。
- 具体武器的状态、输入包、解析和动画通知必须归属 `Instance/<Weapon>/`。
- 角色只能向 `ABBBEquipment::SubmitCommand` 提交通用命令并显式提供 `bIsMirror`；武器实现负责将其映射为公共固定输入包。
- 装备的 Causal 路径可以生成事实，Mirror 路径只能还原事实和表现，禁止重新产生因果结果。
