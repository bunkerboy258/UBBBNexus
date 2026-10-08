# Character Config Law

## 目录职责
- Aim/ 存放角色瞄准配置。
- Animation/ 存放角色动画配置。
- Item/ 存放通用物品定义 统一目录引用 容量 快捷数量与穿戴物品位置配置。
- Appearance/ 存放基础身体 无装备回退与共享外观资源类型。
- Equipment/ 存放装备挂接配置 装备类由统一 Item/Catalog/ 检索。
- Locomotion/ 存放角色移动配置。
- Network/ 存放角色网络配置。
- 背包容量与快捷槽位数量只在 Item/ 中定义 禁止在 Equipment/ 保留旧字段或重复配置。
- 禁止在 Character/Config/ 新增直属文件夹或文件。
