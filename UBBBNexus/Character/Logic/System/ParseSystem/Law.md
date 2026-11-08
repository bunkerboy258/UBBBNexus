# Character Parse System Law

- Parse System 是角色固定输入槽位的唯一消费者
- Processor 只按源码顺序调用输入包的 `CanApply/Apply` 禁止知道具体包的内部规则

## 重要
- 输入包必须是无状态的 不应包含任何副作用
- 输入包的`IsValid``CanApply`只能只读角色的黑包含状态进行判断
- 输入包的`Apply`只能直接写入角色黑板状态数据 可以使用无副作用的值计算 禁止调用系统处理器或 UE 执行接口
- 蒙太奇包的`Apply`只能写入所属固定槽位的蒙太奇引用及其待消费标记
- 输入包的实现处禁止拥有网络语义
