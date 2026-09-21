# Character Controller Law

严格遵守 `Constitution/Data/Law.md` 的约定

控制器根位于控制器目录下 以下控制器根目录统一使用 `X/` 表示

- 控制器根只能拥有初始化函数和一个无参 `Update`
- `Update` 只负责组装 Context 并按固定顺序调度处理器
- 控制器根禁止持有具体领域逻辑
- 控制器只能由 `Character/Core/Update` 下的主管线调用

物理目录固定为

- `X/` 直属第一级只允许控制器根对应的 `.h` `.cpp` 与 `Law.md`
- `X/DomainData/` 放且只放 B
- `X/DomainData/States/` 放且只放 A
- `X/DomainData/Context/` 放且只放 D
- `X/Processors/` 放具体处理器

数据结构要求

- B 必须直接持有本控制器领域内全部 A
- 禁止创建 B 到总 State 再到具体状态的中间聚合层
- `States/` 中所有类型必须以 `State` 结尾
- 以上目录禁止继续创建子目录或其它同级目录

处理器要求

- 每个处理器只处理一个任务
- 处理器之间禁止直接互相调用 只能通过 Context 传递本次调用数据
- 一个处理器文件只能拥有一个 `Update`
- 处理器 `.cpp` 超过二百行时必须重新检查职责是否需要拆分

除了装备控制器读取执行模式并选择 Causal 或 Mirror 路径外 控制器禁止拥有网络语义和具体网络逻辑
