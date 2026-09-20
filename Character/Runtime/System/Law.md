有且仅有一个公共无参方法“Update”  只能由Character/Core/Update下的主管线调用

`State/` 放跨帧驻留的黑板状态 类型名以 `State` 结尾

`Context/` 放执行期间的临时上下文 类型名以 `Context` 结尾

`Definition/` 放本领域的自定义类型定义 类型名以 `Definition` 结尾

网络系统只观察黑板 State 与本帧事实后传输 接收端只能构造领域输入包并调用 SubmitInput 禁止直接写黑板

输入系统禁止拥有网络身份 客户端预测 还原或传输语义 网络只传输已经成立的事实与最终状态
