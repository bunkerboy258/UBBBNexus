# Character System Law

每个 System 有且仅有一个公共无参方法 `Update`，只能由 `Character/Core/Update` 下的主管线调用

`State/` 放跨帧驻留的黑板状态 类型名以 `State` 结尾

`Context/` 放执行期间的临时上下文 类型名以 `Context` 结尾

`Definition/` 放本领域的自定义类型定义 类型名以 `Definition` 结尾

`Processors/` 放处理具体逻辑的方法 类型名以 `Processor` 结尾

以上四种职责目录禁止再创建子文件夹和同级职责目录

角色主管线每帧生成唯一 `NetworkIdentity` 事实，玩法系统只按 `Causal/Mirror` 执行模式工作，禁止自行查询网络角色

只有网络系统可以读取权威身份并决定传输方向，网络系统只观察黑板 State 与本帧事实后传输

接收端只能构造领域输入包并调用 `SubmitInput`，禁止直接写黑板，禁止客户端预测，网络只传输已经成立的事实与最终状态
