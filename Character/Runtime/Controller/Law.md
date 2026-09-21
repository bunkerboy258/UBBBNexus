# Character Controller Law
严格遵守Constitution/Data/Law.md的约定

控制器根位于系统根目录下 以下根目录用X/替代
控制器根只能拥有初始化函数和一个无参Update();`Update`负责调度各个处理器,
组装context,禁止持有任何具体领域逻辑,且只能由 `Character/Core/Update` 下的主管线调用

`X/DomainData/` 放B
`X/DomainData/States/` 放A
`X/DomainData/Context/` 放D
`X/Processors/` 放具体处理器
处理器处理单一任务 处理器之间禁止直接互相调用 只能通过context传递数据
一个处理器文件只能持有一个Update方法 当处理器cpp文件代码行数超过200行 需要考虑拆分成多个处理器

除了装备控制器的镜像判断外,控制器没有任何网络语义和具体逻辑

以上文件夹禁止再创建子文件夹和同级目录
