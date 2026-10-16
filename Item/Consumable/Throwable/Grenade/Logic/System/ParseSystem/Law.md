# Grenade Parse System Law

- 系统根只按固定顺序调度本领域处理器
- 系统根在栈上组装本领域的本次 `Context` 并调度处理器 不跨帧持有 `Context`
- `DomainData/` 只保存 B `DomainData/States/` 只保存 A
- B 直接私有持有 A 并提供对应的 `const` 读取方法
- 本次使用的内部状态由显式 `friend` 处理器通过 `RuntimeData` 访问

## 物理目录

```text
ParseSystem/
├─ BBBGrenadeParseSystem.h / .cpp
├─ DomainData/
│  ├─ BBBGrenadeParseDomainState.h
│  ├─ Context/
│  │  └─ BBBGrenadeParseUpdateContext.h
│  ├─ Definitions/
│  │  └─ BBBGrenadeInputSlot.h
│  └─ States/
│     └─ BBBGrenadeInputState.h
└─ Processors/
   └─ BBBGrenadeParseProcessor.h / .cpp
```
