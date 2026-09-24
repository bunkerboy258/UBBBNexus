# Character Config Law

角色配置目录只保存角色静态配置及其聚合入口 禁止运行时状态 网络传输状态和执行逻辑

## 目录职责
- `Aim/` 存放角色瞄准静态配置
- `Animation/` 存放角色动画静态配置
- `Equipment/` 存放角色装备静态配置
- `Locomotion/` 存放角色移动静态配置
- `Network/` 存放角色网络静态配置
- 禁止在 `Character/Config/` 新增直属文件夹或文件
