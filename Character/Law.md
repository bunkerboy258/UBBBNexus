# Character Law

- 所有角色领域数据同时受 [`../Constitution/Data/Law.md`](../Constitution/Data/Law.md) 约束。

- `ABBBCharacter`是角色实例的唯一根载体,具体的逻辑必须下沉
- 通用角色输入只能通过 BBBCharacter 的 SubmitInput 方法提交 装备持有关系由 EquipmentSystem 的选择入口维护 禁止外部直接修改黑板
- 角色代码禁止依赖具体外部实例类型 绝对禁止拥有任何装备的具体实现逻辑和语义

## 目录职责

- `Animation/` 存放角色动画实例与动画接口
- `Appearance/` 存放角色外观数据
- `Config/` 存放角色及各角色子系统的配置
- `Input/` 存放角色输入入口、输入包与输入分类
- `Logic/` 存放角色逻辑核心、运行时数据与领域系统
- `Network/` 存放角色网络组件与网络复制数据
- 禁止在 `Character/` 根目录新增直属文件夹或文件
