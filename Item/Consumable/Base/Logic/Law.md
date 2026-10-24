# Consumable Logic Base Law

- 生命周期基类固定放在 `Core/Initialization/` `Core/Update/` `Core/Shutdown/` 每个目录只保存对应的一组 `.h/.cpp`.
- 具体消耗品实例使用相同的 `Logic/Core/` 三个目录 其生命周期实现必须继承对应基类并声明为 `final`.
- 本目录不新增其它生命周期目录或实现文件.
- 使用期间由角色已有更新管线推进消耗品逻辑 不创建独立 Tick
- Shutdown 清理本次使用的内部状态 不保留跨次使用的持续持有状态
