# Character Config Law

## 目录职责
- Aim/ 存放角色瞄准配置.
- Animation/ 存放角色动画配置.
- Item/ 存放角色物品包装配置 四类目录引用 容量 快捷数量与穿戴位置 不定义外侧物品.
- Appearance/ 存放角色基础身体与无装备回退 物品静态资源类型属于外侧 Item/Wearable/.
- Interaction/ 存放角色持有物品的挂接配置 实例类由四类外侧目录提供.
- Locomotion/ 存放角色移动配置.
- Network/ 存放角色网络配置.
- 背包容量与快捷槽位数量只在 Item/ 中定义 禁止在 Interaction/ 保留旧字段或重复配置.
- 禁止在 Character/Config/ 新增直属文件夹或文件.
