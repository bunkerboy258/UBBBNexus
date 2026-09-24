# Character Equipment System Law

- 本系统只负责通用装备容器 选择 挂接及生命周期
- 选择请求通过系统入口提交 在固定 Update 阶段消费
- 禁止开火 换弹 弹药 动作事实和具体网络传输逻辑
- 装备自主读取持有者身份 角色不得向行为输入附加镜像参数

## 目录职责
- `Context/` 当前为空 更新临时数据存放在 `DomainData/Context/`
- `Definition/` 当前只含空的 `Commands/` `Events/` 与 `States/` 子目录
- `DomainData/` 存放装备领域状态持有者 定义 状态与更新上下文
- `Processors/` 存放装备选择与生命周期处理器
- 禁止在 `Character/Logic/System/EquipmentSystem/` 新增直属文件夹或文件
