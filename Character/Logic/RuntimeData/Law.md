# Character Runtime Data Law

严格遵守 `Constitution/Data/Law.md` 的约定

- `FBBBCharacterRuntimeData` 是角色运行时实例公开持有的唯一 C
- C 只能直接持有各领域 B 禁止直接持有 A D 或普通状态字段
- 来自角色外部的 B 定义放在 `ExternalDomain/` 下
- 来自角色外部的 A 定义放在 `ExternalDomain/States/` 下
- `ExternalDomain/` 中的 B 必须直接持有全部外部 A 禁止增加总 State 聚合层
- 外部状态只能由角色主管线维护
