# Grenade Use System Law

- 系统根按 Use 与 Finish 的固定顺序调度本领域处理器
- 系统根在栈上组装本领域的本次 Context 并调度处理器 不跨帧持有 Context
- DomainData/ 只保存 B DomainData/States/ 只保存 A
- B 直接私有持有 A 并提供对应的 const 读取方法
- 内部使用状态由本系统自己的处理器通过 RuntimeData 维护
- Use 根据角色输入维护本次使用状态 并向角色贡献蒙太奇输入
- 释放与结束由对应通知触发 Finish 清理本次使用状态
- 本系统不直接生成世界投掷物 不维护背包数量 不承担独立网络同步

## 物理目录

```text
UseSystem/
    BBBGrenadeUseSystem.h / .cpp
    DomainData/
        BBBGrenadeUseDomainState.h
        Context/
            BBBGrenadeUseUpdateContext.h
        States/
            BBBGrenadeUseInputState.h
            BBBGrenadeUseState.h
    Processors/
        BBBGrenadeFinishProcessor.h / .cpp
        BBBGrenadeUseProcessor.h / .cpp
```
