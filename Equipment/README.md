# Equipment

`ABBBEquipment` 是所有装备共享的抽象演员基座，只保存配置、网格、动画实例、镜像身份和固定行为入口。

具体装备直接位于 `Equipment/<Weapon>/`。`ABBBRifleEquipment` 本身就是步枪唯一运行时根，同时持有 `FBBBRifleRuntimeData` 与当前帧固定输入；不存在额外的 `Instance`、`Runtime`、`Signature` 或 `DomainState` 包装层。

角色装备系统只负责装备生命周期并调用 `SubmitEquipInput`、`SubmitPrimaryInput`、`SubmitSecondaryInput`、`SubmitReloadInput` 和 `SubmitRestoreFact`。步枪动画通知调用步枪自己的弹匣与换弹结束输入。

步枪直接重写 `Tick`，在 `TG_PostUpdateWork` 中按固定顺序解析输入包，不经过装备基类 Tick 跳转。每种输入包自行定义合法性、应用条件与对 `RuntimeData` 的修改；镜像实例只消费已经确认的事实，不重新产生玩法因果。角色换弹蒙太奇通过角色输入入口贡献，步枪开火和换弹蒙太奇由装备动画实例直接播放。

`Template/Definition` 只保存纯数据头文件；只有具有实际行为的演员、动画实例、目录和输入包提供 `.cpp`。
