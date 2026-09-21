# Character Update Law

- `Update` 在 CMC 前执行，固定顺序为世界与身份快照、输入解析、装备、Causal 控制器、网络观察。
- `LateUpdate` 必须等待 CMC 完成，只负责动画事实采集和帧末清理。
- `Causal/Mirror` 只在主管线选择玩法执行路径；禁止使用 `HasAuthority` 代替玩法执行身份。
- Tick 前置依赖只能在本目录集中注册和注销，禁止由领域系统私自改变主管线时序。
- 新增主管线阶段必须明确位于输入解析、CMC 和骨骼动画三者中的哪一个边界。
