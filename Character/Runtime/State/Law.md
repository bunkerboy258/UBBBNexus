# Character Runtime State Law

- 本目录的数据构造与写权限同时受 [`../../../DomainData/Law.md`](../../../DomainData/Law.md) 约束。

- 本目录只保存跨多个角色领域共享且允许跨帧驻留的黑板根状态。
- `Definition/` 保存共享事实的数据类型定义；类型名必须以 `Definition` 结尾。
- `NetworkIdentity` 每帧只能由角色主管线刷新，其它对象只能读取。
- 黑板不得主动执行玩法、网络传输或动画播放，不得反向调用控制器与系统。
- 帧末清理只能清除明确声明为瞬时的数据，禁止重置尚未解析的输入槽位。
