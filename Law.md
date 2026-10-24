# UBBBNexus Root Law

本 Law 固定 UBBBNexus 源码仓库的根目录分类

## 根目录结构

仓库根目录直属文件只允许规范文件 与 `README.md`

仓库根目录直属文件夹固定为以下结构

```text
UBBBNexus/
    Character/      角色对象及角色运行逻辑
    Item/           全局物品定义与物品实例
    Player/          本地玩家控制与玩家界面
        Input/      玩家输入适配
        Camera/     玩家相机对象与相机逻辑
        UI/         玩家界面
    Mass/            Mass 实体与批处理逻辑
    Notify/          动画通知与表现入口
    Debug/           调试与测试代码
        Character/  角色测试与调试
        Item/       物品测试与调试
        Player/     玩家测试与调试
        Mass/       Mass 测试与调试
```

- 禁止在仓库根目录新增上述结构以外的直属文件夹或文件
- `Player/` 直属代码只放玩家控制器 玩家状态及跨玩家功能共享的数据查询
- `Player/Input/` 只放玩家输入适配及输入配置
- `Player/Camera/` 只放玩家相机对象 配置 输入与处理逻辑
- `Player/UI/` 只放玩家界面及界面展示数据
- `Debug/` 的直属文件夹固定为 `Character/` `Item/` `Player/` `Mass/`
- 测试与调试代码按其主要作用对象归入对应目录
- `Character/` `Item/` `Mass/` 与 `Notify/` 的内部结构遵循各自直属 Law
- 本 Law 只规定根目录归属 不替代对象内部的目录职责与实现规则
