# Consumable Base Law

- 本目录只定义消耗品的公共运行时根与共用持有组件，禁止放入具体消耗品业务。
- 共用静态定义、输入包与网络桥接分别放入 `Config/`、`Input/` 与 `Network/`。
- 对外输入统一通过消耗品根的 `SubmitInput(...)` 进入实例逻辑。

## 物理目录

固定为以下结构
```text
Base/
    BBBConsumable.h / .cpp
    Config/
    Input/
    Network/
    Logic/
        Core/
            Initialization/
            Update/
            Shutdown/
```
- 具体消耗品实现要求看各子文件夹的law.md
