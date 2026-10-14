# Grenade Use System Law

- 系统根按 `Use`、`Launch`、`Finish` 的固定顺序调度本领域处理器。
- 系统根在栈上组装本领域的本次 `Context` 并调度处理器，不跨帧持有 `Context`。
- `DomainData/` 只保存 B，`DomainData/States/` 只保存 A。
- B 直接私有持有 A 并提供对应的 `const` 读取方法。
- 持久状态由显式 `friend` 处理器通过 `RuntimeData` 访问。
- `Launch` 在控制端确定释放结果并消耗一次数量 房主根据有效结果生成独立物理投掷物 客机不得生成第二个因果实体
- 生成去重使用物品代次和释放序号 晚到的释放结果不得作用于新实例

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
