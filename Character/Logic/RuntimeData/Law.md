# Character Runtime Data Law

严格遵守 `Constitution/Data/Law.md` 的约定

- `FBBBCharacterRuntimeData` 是角色的 C
- 来自角色外部的 B 定义放在 `ExternalDomain/` 下
- 来自角色外部的 A 定义放在 `ExternalDomain/States/` 下
- 禁止在 `Character/Logic/RuntimeData/` 新增直属文件夹或文件

- FBBBCharacterRuntimeData 直接持有 ItemSystem 与 EquipmentSystem 各自独立的 DomainState。
- 背包与物品栏选择状态只属于 ItemSystem 的 DomainState。
- 实际装备关系与镜像装备还原状态只属于 EquipmentSystem 的 DomainState。
- 禁止在两个领域之间重复保存同一份背包 快捷选择或目标主手状态。
