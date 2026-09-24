# Character Parse System Law

- Parse System 是角色固定输入槽位的唯一消费者，也是输入包应用顺序的唯一声明位置。
- 处理顺序固定为角色最终状态 连续控制 跳跃 蒙太奇槽位和相机表现。
- Processor 只按源码顺序调用输入包的 `CanApply/Apply`，禁止知道具体包的内部规则。
- 解析期间禁止重入提交输入；解析后提交的新输入必须保留到下一次解析。
- `DomainData/States/` 只保存固定输入和跨帧解析状态 A 禁止再次汇总全部解析状态的总 State
- `DomainData/Context/` 只保存本次解析引用 不得跨帧持有

## 目录职责
- `Context/` 当前为空 输入解析临时数据存放在 `DomainData/Context/`
- `DomainData/` 存放输入解析状态持有者 输入槽定义 持久状态与更新上下文
- `Processors/` 存放角色输入包解析与应用处理器
- `State/` 当前为空 持久状态存放在 `DomainData/States/`
- 禁止在 `Character/Logic/System/ParseSystem/` 新增直属文件夹或文件
