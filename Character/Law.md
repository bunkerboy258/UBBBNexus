# Character Law

- 角色 Actor 同时受 [Actor 宪法](../Constitution/Actor/Law.md) 约束。
- 所有角色领域数据同时受 [`../Constitution/Data/Law.md`](../Constitution/Data/Law.md) 约束。

- `ABBBCharacter`是角色实例的唯一根载体,具体的逻辑必须下沉
- 通用角色输入只能通过 BBBCharacter 的 SubmitInput 方法提交 装备持有关系由 EquipmentSystem 的选择入口维护 禁止外部直接修改黑板
- 角色代码禁止依赖具体外部实例类型 绝对禁止拥有任何装备的具体实现逻辑和语义

## 目录职责

- `Animation/` 存放角色动画实例
- `Config/` 存放角色及各角色子系统的配置
- `Input/` 存放角色输入入口、输入包与输入分类
- `Logic/` 存放角色具体运行时逻辑
- `Network/` 存放角色网络组件(提供网络复制能力)与网络复制数据(网络数据定义)(具体逻辑下沉在系统内)
- 禁止在 `Character/` 根目录新增直属文件夹或文件
