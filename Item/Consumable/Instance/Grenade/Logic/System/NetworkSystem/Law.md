# Grenade Network System Law

- 系统根只按固定顺序调度本领域处理器。
- 系统根在栈上组装本领域的本次 `Context` 并调度处理器，不跨帧持有 `Context`。
- `DomainData/` 只保存 B，`DomainData/States/` 只保存 A。
- B 直接私有持有 A 并提供对应的 `const` 读取方法。
- 持久状态由显式 `friend` 处理器通过 `RuntimeData` 访问。

## 物理目录

```text
NetworkSystem/
├─ Law.md
├─ BBBGrenadeNetworkSystem.h / .cpp
├─ DomainData/
│  ├─ BBBGrenadeNetworkDomainState.h
│  ├─ Context/
│  │  └─ BBBGrenadeNetworkUpdateContext.h
│  └─ States/
│     └─ BBBGrenadeNetworkObservationState.h
└─ Processors/
   └─ BBBGrenadeNetworkProcessor.h / .cpp
```

- 网络同步只传递使用和释放结果 房主据此生成独立投掷物 客机还原角色表现
- 释放数据包含实际释放变换及持有者速度 运动复制由投掷物的 UE 原生物理复制负责
