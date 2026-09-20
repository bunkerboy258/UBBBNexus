# Equipment Law

- `ABBBEquipment` 只承载演员、网格、生命周期和唯一的动态命令边界，不保存武器行为状态。
- 具体武器的状态、输入包、解析和动画通知必须归属 `Instance/<Weapon>/`。
- 角色只能向 `ABBBEquipment::SubmitCommand` 提交通用命令；武器实现负责将其映射为静态输入包。
