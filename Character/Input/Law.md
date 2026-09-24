# Character Input Law

- 所有角色输入包只能通过 `ABBBCharacter::SubmitInput` 进入固定输入黑板。
- 每种输入包必须实现 `IsValid`、`CanApply` 和 `Apply`，包本身完整声明通过条件与黑板效果。
- 每种包在 `FBBBCharacterInputState` 中只能拥有一个固定槽位；同类输入使用激活标记和最新数据覆盖。
- 禁止使用动态队列、运行时类型表、虚函数层级或堆分配保存角色输入。
- 输入包禁止拥有网络发送、RPC、客户端预测或角色网络身份判断语义。
- 输入应用顺序只能由 `FBBBCharacterInputProcessor::Update` 的源码顺序表达，禁止另建优先级数据。

- 包目录第一级为 Local Mirror Shared 第二级为输入类型 第二级目录直接放包 禁止第三级分类
- 每个可独立开始或结束的动作都要有自己的输入包与固定槽 连续移动包只保存移动和朝向 禁止捆绑多个动作
- 每个蒙太奇 Slot 独占一种输入包与头文件 包只保存蒙太奇引用 空引用表示清除此 Slot
- 禁止装备输入与共同蒙太奇数据包装

## 目录职责
- `Local/` 存放本机控制生成的角色输入包
- `Mirror/` 存放网络镜像转换的角色状态输入包
- `Shared/` 存放本机与镜像共用的角色表现输入包
- 禁止在 `Character/Input/` 新增直属文件夹或文件
