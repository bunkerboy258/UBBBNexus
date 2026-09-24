# Character Equipment System Law

- 本系统只负责角色侧对装备的生命周期管理 对装备的单向通信 维护角色的物品容器
- 只能调用装备的抽象方法 禁止持有任何装备的具体逻辑语义或依赖
- 装备禁止与本系统发生交互
- 禁止在 `Character/Logic/System/EquipmentSystem/` 新增直属文件夹或文件
