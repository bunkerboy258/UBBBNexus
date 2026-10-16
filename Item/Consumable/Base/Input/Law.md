# Consumable Base Input Law

- 沿用 Actor 宪法中输入包的纯数据与 SubmitInput 约定 不要求消耗品成为 Actor 或独立网络对象
- 使用 CRTP 模式 本目录只提供两分支共有的使用输入接口与结构模板
- 模板不实现 IsValid CanApply Apply 的运行逻辑 也不提供转发到 Impl 方法的实现
- 具体输入包完整实现上述三个方法 并声明为 final
- 分支专属输入由对应分支定义 不要求另一分支实现无关输入
- 不保留手持 Actor 显示挂接输入或独立网络输入
