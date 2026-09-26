# Character Animation System Law

- Animation System 只能在 CMC 完成后由 `LateUpdate` 调用
- 动画系统禁止生成玩法事实,修改 CMC等具体行为
- 动画系统不得持有任何网络语义
- 输入系统批准的蒙太奇请求 只能写入蒙太奇状态的对应固定槽位引用与待消费标记 蒙太奇处理器消费请求并交给动画实例注册
- 动画领域蒙太奇状态每个固定槽位 只能保存一个蒙太奇引用和待消费标记
- 禁止在 `Character/Logic/System/AnimationSystem/` 新增直属文件夹或文件
