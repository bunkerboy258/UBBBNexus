# 角色物品与外观接口

## 物品只读结果

从 `Character.RuntimeData.Item.ReadItemInventoryState()` 读取:

- `Slots` 是唯一真实物品数组。
- `[0 BackpackSlotCount)` 是背包 前序快捷区域仍由 ItemBarState 定义。
- 穿戴格子全局下标为 `BackpackSlotCount + WearSlots.IndexOfByKey(位置名)`。
- 穿戴位置依次为 Helmet Body Vest Boots Backpack Gloves Attachments Flag。
- `Slots[Index].Definition` 与 `InstanceId` 表示物品及单件身份。
- 穿戴品和杂物的 `EquipmentInstance` 为空是正常状态。
- 同一实例只存在一个格子 穿戴后释放原背包格子。
- 不能用 Actor 有效性判断物品占用 不能以型号替代单件身份。

从 `ReadItemBarState()` 读取选中快捷格子与目标主手装备。
从 `ReadItemOperationState()` 读取完成版本与成功失败数量。
实际主手装备继续由 `Character.GetActiveEquipment()` 读取。

## 物品输入

所有请求提交到角色 `SubmitInput`:

- `FBBBItemAddLocalControlPacket.ItemIds` 是通用型号数组。
- `FBBBItemMoveLocalControlPacket.Sources Targets Instances` 必须等长。
- Instances 是提交时观察到的源实例身份 请求消费前发生变化则拒绝。
- Target 为 INDEX_NONE 时查找首个空背包格子 满包拒绝。
- 普通移动 交换 穿戴与脱下统一使用移动包。
- 交换必须满足两件物品各自的目标位置许可。
- `FBBBItemSelectLocalControlPacket.Slots` 仍选择前序主手格子。
- 选择穿戴品或杂物拒绝 选中格子被换为非装备时取消主手选择。
- 输入接受不表示操作已经成功 UI 应观察完成状态。

## 外观只读与输入

从 `RuntimeData.Appearance.ReadAppearanceSelectionState().Snapshot` 读取完整既成事实。
从 `ReadAppearanceDisplayState()` 读取加载与蓝图应用结果。
从 `ReadAppearanceStyleState()` 读取按实例关联的染色迷彩和基础选择。

独立本机输入均位于 Character/Input/LocalControl/Appearance/:

- FBBBCharacterAppearanceColorLocalControlPacket: Instances Slots Colors 等长 无效实例身份表示基础部件。
- FBBBCharacterAppearanceCamouflageLocalControlPacket: Instances Slots Values 等长。
- FBBBCharacterAppearanceBaseLocalControlPacket: Slots Resources 只引用角色配置中的非物品基础资源。
- FBBBCharacterAppearanceDirtLocalControlPacket 与 FBBBCharacterAppearanceWeatheringLocalControlPacket: Values 范围为 0 到 1。
- 国旗通过 Flag 物品格子选择 不提供绕过持有关系的贴片选择入口。

## 蓝图显示边界

ABBBCharacter 的 `ApplyAppearanceDisplay(Parts)` 是 BlueprintImplementableEvent。
每个显示部件包含 Slot Mesh Materials Attachments 与回退标识。
蓝图只清理旧显示 设置模型材质及挂接附件 不重新判断穿戴或搭配。
国旗材质与裤腿选择已经由 C++ 生成。
UI 人物预览可机械复制正式人物的显示结果 不创建另一份真实物品容器。

## 迁移要求

旧 GetBackpackItems 的 Actor 数组不能继续表示通用物品。
旧 GetItemDefinition 应改为通用 UBBBItemDefinition。
Player 控制器 UI 旧外观会话和 Client 外观存档消费方由 UI 任务迁移。
删除 UBBBAppearanceComponent 后不得保留转发或兼容组件。
完整外观存档不能直接恢复正式穿戴部件 本轮没有背包持久化。

## 既有装备资产的型号迁移

统一定义中的 `ItemId` 沿用下表型号。现有装备定义的旧 `EquipmentId` 字段删除后 资产必须按表明确写入新字段并保存 不使用属性跳转或兼容读取。

| 现有定义 | 新 ItemId |
| --- | --- |
| Equipment/Rifle/Rifle_01/DA_ModernWeapons_Rifle_01 | ModernWeapons_Rifle_01 |
| Equipment/Rifle/Rifle_02/DA_ModernWeapons_Rifle_02 | ModernWeapons_Rifle_02 |
| Equipment/Rifle/Rifle_03/DA_ModernWeapons_Rifle_03 | ModernWeapons_Rifle_03 |
| Equipment/Rifle/Rifle_04/DA_ModernWeapons_Rifle_04 | ModernWeapons_Rifle_04 |
| Equipment/Rifle/Rifle_05/DA_ModernWeapons_Rifle_05 | ModernWeapons_Rifle_05 |
| Equipment/Rifle/Rifle_06/DA_ModernWeapons_Rifle_06 | ModernWeapons_Rifle_06 |
| Equipment/Melee/Bat_01/DA_Bat_01 | Bat_01 |

表中定义路径均位于 `/Game/_Project/Characters/BBBC_UA/`。
