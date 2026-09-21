# Character Runtime Law

- `RuntimeData/` 保存角色跨领域共享的运行时数据根，`Controller/` 产生本机玩法结果，`System/` 执行解析、网络和动画边界工作。
- 运行时对象只能由 `Character/Core/Initialization` 装配，只能由主管线调用公开更新入口。
- 领域对象只读取已注入依赖，禁止在更新期间自行搜索角色组件或重新构造系统。
- 跨领域通信优先写入明确黑板或提交输入包，禁止通过隐式回调链传递玩法结果。
