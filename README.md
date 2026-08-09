# UBBBNexus

UBBBNexus 是面向 Unreal Engine 5.6 的多人第三人称角色、装备与物品架构基础。目前作为 `ABBB_Evac` 工程中的运行时源码边界开发，并以清晰的数据流、严格的职责划分和长期可维护性为主要目标。

## 目录

- `Character/`：角色黑板、控制管线、领域系统、动画表现与网络桥接
- `Equipment/`：装备实例、装备系统、表现实体和能力 Fragment
- `Item/`：基础物品定义及投射物等物品实现
- `Player/`：玩家控制器与玩家状态

## 角色架构

角色由固定顺序的 Pipeline 和按领域内聚的 System 共同驱动：

```text
Input → Intent → Request → Arbitration → Execution
      → Equipment / Camera / Aim / Locomotion / Facing
      → Network / Animation → RuntimeData Clean
```

- Pipeline 负责阶段明确的数据转换和请求处理
- System 负责稳定维护一个领域的状态并响应角色黑板
- RuntimeData 是角色长期状态、帧数据和跨系统事实的统一数据根
- `FBBBCharacterInitializer` 在初始化阶段注入依赖
- `FBBBCharacterUpdatePipeline` 根据角色控制身份调度固定更新路径

AimSystem 仅在本地控制角色上生成瞄准事实，并按移动状态分流身体朝向处理。远端角色依赖 UE Character Movement 同步位置和旋转，AnimationSystem 根据同步后的世界事实重建动画偏角与 IK 表现。

## 装备与物品

装备实例是装备的运行根，持有配置、运行数据、表现实体与装备系统。装备能力通过显式 Fragment 插槽组合，当前覆盖装备、开火、弹匣、换弹、动画姿态和左手 IK 等领域。

角色侧只维护物品容器、期望装备和当前装备，并通过装备实例的公开能力发起操作；弹药、射击间隔、投射物生成和换弹规则由装备自身负责。

## 网络模型

角色网络采用 UE Listen Server 主机模型，并坚持本地因果与最小权威校验：

- `UBBBCharacterNetworkComponent` 只提供 RPC 与属性复制能力
- `FBBBCharacterNetworkSystem` 持有网络处理逻辑
- Uploader 观察黑板并生成上行数据
- Validator 在权威端校验需要校验的数据
- Restorer 将保留的数据还原到本地黑板或装备实例
- 位置和角色旋转交给 UE Character Movement 同步
- 连续瞄准状态只传输必要目标数据，各端自行重建动画表现

## 当前阶段

当前已完成角色数据黑板、输入到执行管线、主要角色系统、装备 Fragment 组合、基础开火与换弹链、网络上传/校验/还原链，以及代码到动画蓝图的只读表现桥接。

项目仍处于架构筑基与联机验证阶段，后续重点是继续收紧数据访问边界、完善装备能力、验证多人状态一致性并清理历史遗留实现。

## 宿主工程

当前开发宿主为 Unreal Engine 5.6 工程 `ABBB_Evac`。在宿主工程中的集成位置为：

```text
Source/ABBB_Evac/BBBWork/UBBBNexus
```
