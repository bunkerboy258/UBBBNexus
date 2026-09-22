# Equipment

`ABBBEquipment` 是所有装备共享的抽象演员基座，只保存配置、网格、动画实例、镜像身份和固定行为入口。

具体装备直接位于 `Equipment/<Weapon>/`。`ABBBRifleEquipment` 本身就是步枪唯一运行时根，公开持有唯一的 `FBBBRifleRuntimeData`。不存在额外的 `Instance`、`Runtime` 或 `Signature` 对象；黑板严格按数据宪法持有领域状态：

```text
ABBBRifleEquipment
└─ RuntimeData / FBBBRifleRuntimeData
   └─ Rifle / FBBBRifleDomainState
      ├─ Action / FBBBRifleActionState
      └─ Input / FBBBRifleInputState
```

`RuntimeData/` 保存 C，`DomainData/` 保存 B，`DomainData/States/` 保存 A。两个 State 分别保存动作事实和待解析输入，由 B 私有构造并直接持有；对外只提供 `ReadRifleActionState()` 与 `ReadRifleInputState()`。没有黑板外的 `PendingInputs` 成员。

角色装备系统只负责装备生命周期并调用 `SubmitEquipInput`、`SubmitPrimaryInput`、`SubmitSecondaryInput`、`SubmitReloadInput` 和 `SubmitRestoreFact`。步枪动画通知调用步枪自己的弹匣与换弹结束输入。

步枪直接重写 `Tick`，在 `TG_PostUpdateWork` 中按固定顺序解析输入包，不经过装备基类 Tick 跳转。每种输入包自行定义合法性、应用条件与对 `RuntimeData` 的修改；镜像实例只消费已经确认的事实，不重新产生玩法因果。角色换弹蒙太奇通过角色输入入口贡献，步枪开火和换弹蒙太奇由装备动画实例直接播放。

`Template/Definition` 只保存纯数据头文件；只有具有实际行为的演员、动画实例、目录和输入包提供 `.cpp`。

`DomainData/Context/BBBRifleInputContext.h` 只保存一次解析的依赖，在步枪 Tick 栈上构造；共用的蒙太奇和声音操作位于 `Processors/BBBRiflePresentationProcessor`。数据容器不承担 Submit、Reset 或表现操作。

跨领域的约束见 `Constitution/Equipment/Law.md`。角色网络系统观察角色侧已经接收的装备事实，镜像角色再通过固定入口交给装备还原。装备不直接发送网络消息。相机贡献也先进入角色输入，再由玩家相机系统消费。

当前功能边界：Rifle Tick 发布换弹、弹药、开火序号和时间事实；左手握持与 AimSource 字段目前没有计算链路，保持默认无效。配置和 getter 的存在不代表这两条链路已接通，本次数据持有修正没有扩展这部分行为。
