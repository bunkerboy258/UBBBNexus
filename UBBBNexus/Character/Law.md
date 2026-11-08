# Character Law

- 角色 Actor 同时受 [Actor 宪法](../../Constitution/Actor/Law.md) 约束.
- 所有角色领域数据同时受 [数据宪法](../../Constitution/Data/Law.md) 约束
- ABBBCharacter 是角色实例的唯一根载体 具体逻辑必须下沉
- 通用角色输入只能通过 ABBBCharacter 的 SubmitInput 方法提交
- 禁止在 Character/ 根目录新增直属文件夹或文件.
