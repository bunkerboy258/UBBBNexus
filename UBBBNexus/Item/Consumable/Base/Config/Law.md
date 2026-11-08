# Consumable Base Config Law

- 本目录只定义消耗品共有的静态使用配置 并继承物品定义基类
- 配置明确区分投掷物与恢复物分支
- 网格与使用动作只保存资源配置 不表示消耗品拥有显示组件或动画实例
- 不创建 .cpp
- 具体消耗品实现的配置必须继承 UBBBConsumableDefinition 并声明为 final
