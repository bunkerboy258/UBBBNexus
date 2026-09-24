# Character System Law

严格遵守 `Constitution/Data/Law.md` 的约定

系统根位于系统目录下 以下系统根目录统一使用 `X/` 表示

- 系统根负责初始化与无参 Update 装备持有系统可以提供固定选择请求入口
- `Update` 只负责组装 Context 并按固定顺序调度处理器
- 系统根禁止持有具体领域逻辑
- 系统只能由 `Character/Logic/Core/Update` 下的主管线调用

物理目录固定为

- `X/` 直属第一级只允许系统根对应的 `.h` `.cpp` 与 `Law.md`
- `X/DomainData/` 放且只放 B
- `X/DomainData/States/` 放且只放 A
- `X/DomainData/Context/` 放且只放 D
- `X/DomainData/Definitions/` 放且只放 O
- `X/Processors/` 放具体处理器

数据结构要求

- B 必须直接持有本系统领域内全部 A
- 禁止创建 B 到总 State 再到具体状态的中间聚合层
- `States/` 中所有类型必须以 `State` 结尾
- 以上目录禁止继续创建子目录或其它同级目录

处理器要求

- 每个处理器只处理一个任务
- 处理器之间禁止直接互相调用 只能通过 Context 传递本次调用数据
- 一个处理器文件只能拥有一个 `Update`
- 处理器 `.cpp` 超过二百行时必须重新检查职责是否需要拆分

装备系统可以读取执行模式并选择 Causal 或 Mirror 路径 但禁止拥有网络语义与具体网络逻辑

只有网络系统允许存在网络语义与具体网络逻辑

## 目录职责
- `AimSystem/` 存放角色瞄准领域系统
- `AnimationSystem/` 存放角色动画层 动作事实与动画实例发布系统
- `AppearanceSystem/` 当前只有 `DomainData/` 与 `Processors/` 空壳目录 尚无实际系统实现
- `EquipmentSystem/` 存放角色通用装备选择 挂接与生命周期系统
- `LocomotionSystem/` 存放角色移动控制应用与移动状态系统
- `NetworkSystem/` 存放角色网络观察与发送调度系统
- `ParseSystem/` 存放角色固定输入槽解析系统
- 禁止在 `Character/Logic/System/` 新增直属文件夹或文件
