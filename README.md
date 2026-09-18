# UBBBNexus

Unreal Engine 5.8 的角色与装备运行时源码 宿主为 `E:\BBB_Evac`

## 结构

- `Character/Input/`：角色定义的固定类型输入与入口 `GetInput().Submit(Packet)`
- `Character/Instance/Pipeline/`：Input 整理与 Arbitration 固定顺序调度 包规则负责黑板效果
- `Character/Instance/System/`：根据黑板维护行为 不读取玩家按键和视点
- `PlayerInput/`：控制器持有的输入组件 负责增强输入、屏蔽、解绑及世界空间数据组装
- `PlayerCamera/`：独立相机 Actor 负责跟随角色与消费相机输入
- `Equipment/`：独立 Tick 与固定系统和 Fragment 配置 本轮仅迁移角色入口引用
- `Player/`：玩家控制器与玩家状态
- `Item/`：投射物等基础实现

## 更新顺序

```text
控制器采样 → PlayerInput → 角色根管线
    网络收件转换 → Input → Arbitration 调用包规则写黑板
    → Equipment → 本地 Aim / Locomotion → 本地网络上传
    → 移动组件 → LateUpdate 动画事实与蒙太奇 → 角色骨骼动画
    → 装备独立 TG_PostUpdateWork → 装备动画快照
```

装备与动画回调只投递收件箱 收件箱不参与角色帧末清理 下一帧统一消费 相机在 TG_PostUpdateWork 跟随角色 不成为输入 Tick 的前置依赖

## 规则

优先级由 C++ 固定为结果还原、已接受反馈、普通请求 调用者不能自行提高优先级

有效切换立即结束原换弹 空快捷槽不取消换弹 换弹优先于开火且换弹期间阻止开火 瞄准优先于冲刺 一次性失败请求不缓存 持续输入保留 没有通用能力任务或效果框架

蒙太奇按实际五个 Slot 保存空或一个蒙太奇 多轨蒙太奇共享播放修订号与一个 UBBBMontagePlayback 沿用资产 Group 互斥 旧播放结束不能清掉新修订号

## 网络

保持本地控制端完成因果、服务器分发、镜像还原的现有模型 RPC 与网络身份只由 NetworkSystem 和根管线处理 传输格式转换为 FBBBCharacterRestoreInput 后通过受限入口投递 还原包只写黑板 镜像不重新发起玩法 位置和旋转继续由 CharacterMovement 同步

## 迁移与检查

见 [角色输入迁移说明](Character/Input/README.md) 本次不自动修改资产 不保留旧接口兼容层

编译目标为 ABBB_EvacEditor Win64 Development 固定冲突规则测试为 BBB.Character.Input.ConflictRules 不替代 PIE 或联机运行验证
