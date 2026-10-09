# 角色物品与外观接口

## 物品只读结果

从 `Character.RuntimeData.Item.ReadCharacterItemInventoryState()` 读取:

- `Slots` 是唯一真实物品数组。
- `[0 BackpackSlotCount)` 是背包 前序快捷区域仍由 ItemBarState 定义。
- 穿戴格子全局下标为 `BackpackSlotCount + WearSlots.IndexOfByKey(位置名)`。
- 穿戴位置依次为 Helmet Body Vest Boots Backpack Gloves Attachments Flag。
- `Slots[Index].Definition` 与 `InstanceId` 表示物品及单件身份。
- 穿戴品和杂物的 `EquipmentInstance` 为空是正常状态。
- 同一实例只存在一个格子 穿戴后释放原背包格子。
- 不能用 Actor 有效性判断物品占用 不能以型号替代单件身份。

从 `ReadCharacterItemBarState()` 读取选中快捷格子与目标主手装备。
从 `ReadCharacterItemOperationState()` 读取完成版本与成功失败数量。
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

从 `RuntimeData.Appearance.ReadCharacterAppearanceSelectionState().Snapshot` 读取完整既成事实。
从 `ReadCharacterAppearanceDisplayState()` 读取资源准备与机械应用状态。
从 `ReadCharacterAppearanceStyleState()` 读取按实例关联的染色迷彩和基础选择。

独立本机输入均位于 Character/Input/LocalControl/Appearance/:

- FBBBCharacterAppearanceColorLocalControlPacket: Instances Slots Colors 等长 无效实例身份表示基础部件。
- FBBBCharacterAppearanceCamouflageLocalControlPacket: Instances Slots Values 等长。
- FBBBCharacterAppearanceBaseLocalControlPacket: Slots Resources 只引用角色配置中的非物品基础资源。
- FBBBCharacterAppearanceDirtLocalControlPacket 与 FBBBCharacterAppearanceWeatheringLocalControlPacket: Values 范围为 0 到 1。
- 国旗通过 Flag 物品格子选择 不提供绕过持有关系的贴片选择入口。

## 蓝图显示边界

ABBBCharacter 的 `ApplyAppearanceDisplay(Parts)` 是 BlueprintNativeEvent 默认由原生代码完成机械应用。
每个显示部件包含 Slot Mesh Materials Attachments 与回退标识 附件按挂点携带 Mesh 和完整 Materials。
蓝图覆盖只能清理旧显示 设置模型材质及挂接附件 并返回是否完整应用成功 不重新判断穿戴或搭配。
应用前核对全部配置组件和附件挂点 缺失或重复映射拒绝目标显示。
PreparedRevision 表示已经准备的目标版本 AppliedRevision 只在完整目标应用成功后前进。
bApplied 表示当前目标成功 bFallbackApplied 表示失败后已对可用组件应用基础回退。
Parts 是准备中的目标结果 FallbackParts 是基础回退结果 两者不能仅凭数组存在判断当前显示成功。
资源或机械应用失败不改变物品位置 同一版本按一秒间隔重试 重复失败不反复输出日志。
未变化的动态材质实例复用 主体和附件均更新染色 迷彩 污渍与磨损。
国旗材质与裤腿选择已经由 C++ 生成。
UI 人物预览可机械复制正式人物的显示结果 不创建另一份真实物品容器。

## 身份与持久化边界

通用物品读取 UBBBItemDefinition 单件身份使用 InstanceId 不以 Actor 数组表示物品容器。
外观存档只保存本领域风格 不直接恢复正式穿戴物品 本轮没有背包持久化。
全部消费方使用当前直接接口 不保留旧组件 转发接口 属性跳转或兼容读取。

## 既有装备型号

统一定义中的 `ItemId` 使用下表型号。

| 定义 | ItemId |
| --- | --- |
| Equipment/Rifle/Rifle_01/DA_ModernWeapons_Rifle_01 | ModernWeapons_Rifle_01 |
| Equipment/Rifle/Rifle_02/DA_ModernWeapons_Rifle_02 | ModernWeapons_Rifle_02 |
| Equipment/Rifle/Rifle_03/DA_ModernWeapons_Rifle_03 | ModernWeapons_Rifle_03 |
| Equipment/Rifle/Rifle_04/DA_ModernWeapons_Rifle_04 | ModernWeapons_Rifle_04 |
| Equipment/Rifle/Rifle_05/DA_ModernWeapons_Rifle_05 | ModernWeapons_Rifle_05 |
| Equipment/Rifle/Rifle_06/DA_ModernWeapons_Rifle_06 | ModernWeapons_Rifle_06 |
| Equipment/Melee/Bat_01/DA_Bat_01 | Bat_01 |

表中定义路径均位于 `/Game/_Project/Characters/BBBC_UA/`。
