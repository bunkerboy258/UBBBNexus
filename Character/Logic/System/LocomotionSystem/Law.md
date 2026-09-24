# Locomotion System Law

- 本系统只把解析后的控制事实应用到角色与 CMC，并形成最终移动状态。
- 所有 CMC 写入必须集中在 Locomotion Processor，禁止其它领域直接产生移动输入。

## 目录职责
- `Definition/` 当前为空 移动领域状态与更新上下文存放在 `DomainData/`
- `DomainData/` 存放移动领域状态持有者 持久状态与更新上下文
- `Processors/` 存放角色移动控制与最终状态处理器
- 禁止在 `Character/Logic/System/LocomotionSystem/` 新增直属文件夹或文件
