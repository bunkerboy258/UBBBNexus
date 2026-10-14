# Grenade Use System Law

- 系统根按 `Use`、`Launch`、`Finish` 的固定顺序调度本领域处理器。
- 系统根在栈上组装本领域的本次 `Context` 并调度处理器，不跨帧持有 `Context`。
- `DomainData/` 只保存 B，`DomainData/States/` 只保存 A。
- B 直接私有持有 A 并提供对应的 `const` 读取方法。
- 持久状态由显式 `friend` 处理器通过 `RuntimeData` 访问。
- `Launch` 只提交 Mass 投掷物出生输入；投掷物后续模拟与伤害由 `Mass/Instance/Projectile/` 负责。

## 物理目录

```text
UseSystem/
├─ Law.md
├─ BBBGrenadeUseSystem.h / .cpp
├─ DomainData/
│  ├─ BBBGrenadeUseDomainState.h
│  ├─ Context/
│  │  └─ BBBGrenadeUseUpdateContext.h
│  └─ States/
│     ├─ BBBGrenadeUseInputState.h
│     └─ BBBGrenadeUseState.h
└─ Processors/
   ├─ BBBGrenadeFinishProcessor.h / .cpp
   ├─ BBBGrenadeLaunchProcessor.h / .cpp
   └─ BBBGrenadeUseProcessor.h / .cpp
```
