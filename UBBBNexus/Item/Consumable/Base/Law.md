# Consumable Base Law

- 本目录只定义消耗品的公共轻量运行时根 静态定义与输入 禁止放入具体消耗品业务
- 消耗品运行时只服务本次使用 不构造持续持有关系
- 本根不拥有网格组件 独立 Tick 动画实例或网络组件
- 对外输入统一通过消耗品根的模板 `void SubmitInput(...)` 进入实例逻辑
- 消耗品内部状态由自身内部逻辑维护

## 物理目录

```text
Base/
    BBBConsumable.h / .cpp
    Config/
    Input/
    Logic/
        Core/
            Initialization/
            Update/
            Shutdown/
```
