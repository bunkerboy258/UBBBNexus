# Consumable Law

- 遵循 `../Law.md` 与数据宪法
- 消耗品不属于 BBBActor 使用轻量 UObject 作为本次使用的运行时根
- 消耗品只能使用 不存在持续持有或一直拿在手中的状态 运行状态只服务本次使用
- 消耗品内部状态由自身内部逻辑维护 根据角色输入向角色贡献使用动作输入
- 消耗品不构成独立网络对象 使用动作 使用结果和释放结果由角色同步
- 消耗品的所有网格显示都通过表现通知生成
- 消耗品不创建自身 Animation/ AnimationSystem 网络组件或 NetworkSystem

## 物理目录

```text
Consumable/
    Base/
    Catalog/
    Throwable/
    Recovery/
    Projectile/
```

- Base/ 是唯一公共基类目录 定义公共运行时根 静态配置和输入 不复用装备 Base/
- Throwable/ 与 Recovery/ 分别定义投掷物和恢复物的使用逻辑 不各自创建第二套 Base/
- 具体实例保留 Config/ Input/ 与 Logic/ 的代码组织
- Logic/ 保留 Core/Initialization/ Core/Update/ Core/Shutdown/ RuntimeData/ 和 System/
- 每种具体消耗品保留 ParseSystem 与使用业务系统 系统仍按数据宪法组织领域状态和处理器
- Projectile/ 保存释放后的独立世界物理 Actor 继续遵循 Actor 与网络宪法
