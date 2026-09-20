# Equipment Catalog Law

- Catalog 只维护 `EquipmentId` 到 `UBBBEquipmentDefinition` 的确定性映射。
- Catalog 禁止创建、附着、更新或销毁装备实例，禁止保存角色与运行时装备状态。
- 未找到配置时必须明确返回失败，禁止隐式创建默认装备或猜测具体装备类型。
