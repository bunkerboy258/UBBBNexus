# Character Animation System Law

- Animation System 只能在 CMC 完成后由 `LateUpdate` 调用
- 动画系统禁止生成玩法事实 修改 CMC等具体行为
- 动画系统不得持有任何网络语义
- 动画专用 Processor 可以只读业务事实与动画配置 维护本动画领域的运行状态并生产动画结果
- 多个动画层共用的角色与装备事实由 AnimationSystem 统一发布
- 计算职责按数据归属和采样时机确定
- Processor 不得依赖具体动画蓝图的图表结构或节点布局
- 需要读取具体动画实例已求值曲线或节点上下文的计算 保留在对应动画更新阶段
- 输入系统批准的蒙太奇请求 只能写入蒙太奇状态的对应固定槽位引用与待消费标记 蒙太奇处理器消费请求并交给动画实例注册
- 动画领域蒙太奇状态每个固定槽位 只能保存一个蒙太奇引用和待消费标记
- 禁止在 `Character/Logic/System/AnimationSystem/` 新增直属文件夹或文件
