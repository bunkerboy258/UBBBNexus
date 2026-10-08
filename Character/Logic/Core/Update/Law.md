# Character Update Law

- Update 在 CMC 前执行。
- 固定顺序为 世界与身份快照 → 输入解析 → ItemSystem → EquipmentSystem → AppearanceSystem → 其它 Causal 玩法系统 → 网络观察。
- ItemSystem 只在 Causal 路径维护真实背包和物品栏选择。
- ItemSystem 必须先完成背包操作 再生成本帧目标主手物品结果。
- EquipmentSystem 必须在 ItemSystem 之后维护本帧实际装备关系。
- AppearanceSystem 位于物品与装备维护之后 CMC 和网络观察之前。
- Mirror 路径不执行真实背包与快捷选择逻辑 EquipmentSystem 只消费接收的持有关系结果。
- LateUpdate 必须等待 CMC 完成 只负责动画事实采集和帧末清理。
- Causal/Mirror 由主管线确定 禁止使用 HasAuthority 代替玩法执行身份。
- Tick 前置依赖只能在本目录集中注册和注销 禁止由领域系统私自改变主管线时序。
- 新增主管线阶段必须明确位于输入解析 CMC 和骨骼动画的哪一个边界。

## 目录职责
- 本目录无直属子目录 直属文件实现角色 Update 与 LateUpdate 主管线。
- 禁止在 Character/Logic/Core/Update/ 新增直属文件夹或文件。
