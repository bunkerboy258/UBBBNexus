# Character Controller Law

- Controller 的依赖注入和 State 写权限同时受 [`../../../DomainData/Law.md`](../../../DomainData/Law.md) 约束。

- Controller 只负责一个明确玩法领域，并通过自己的 Processor 修改该领域状态。
- Controller 的更新入口只能由角色主管线调用，禁止 Controller 之间互相调度。
- Controller 禁止查询 `HasAuthority`、本地控制权、网络角色或 RPC；主管线只向确有需要的边界传入 `Causal/Mirror` 事实。
- Controller 不负责网络发送、动画播放或输入排序，只产生可供后续系统观察的状态与事实。
- `Definition/` 保存本领域数据定义，`Processors/` 保存具体规则，禁止新增含义重复的层级。
