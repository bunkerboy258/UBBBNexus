# Character Parse System Law

- Parse System 是角色固定输入槽位的唯一消费者，也是输入包应用顺序的唯一声明位置。
- 处理顺序固定为最终状态、既成事实、连续控制、离散命令、蒙太奇槽位和相机表现。
- Processor 只按源码顺序调用输入包的 `CanApply/Apply`，禁止知道具体包的内部规则。
- 解析期间禁止重入提交输入；解析后提交的新输入必须保留到下一次解析。
- `DomainData/States/` 保存固定输入和跨帧解析状态，`DomainData/Context/` 只保存本次解析引用，不得跨帧持有。
