# UBBBNexus

Unreal Engine 5.8 的角色与装备运行时源码 宿主为 `E:\BBB_Evac`

## 结构

- `Character/Input/`：角色输入提交闸口与固定槽位帧，同类型输入按最后一次提交覆盖
- `Character/Input/Packets/`：一包一对 h cpp 的输入包 自描述校验 条件与效果
- `Character/Core/`：角色初始化 销毁和根更新调度
- `Character/Runtime/Controller/`：根据黑板维护角色控制行为
- `Character/Runtime/System/`：解析 动画和网络领域系统
- `Character/Runtime/RuntimeData/`：角色跨领域运行时数据根
- `PlayerInput/`：控制器持有的输入组件 负责增强输入、屏蔽、解绑及世界空间数据组装
- `PlayerCamera/`：独立相机 Actor 负责跟随角色与消费相机输入
- `Equipment/`：独立 Tick 与固定系统和 Fragment 配置 本轮仅迁移角色入口引用
- `Player/`：玩家控制器与玩家状态
- `Item/`：投射物等基础实现

## 更新顺序

```text
控制器采样 → PlayerInput → 角色根管线
    网络收件转换 → Input → 显式顺序原地写黑板
    → Equipment → 权威或本机 Aim / Locomotion → 唯一 NetworkSystem.Update
    → 移动组件 → LateUpdate 动画事实与蒙太奇 → 角色骨骼动画
    → 装备独立 TG_PostUpdateWork → 装备动画快照
```

装备与动画回调只覆盖对应固定槽位 解析后的新提交自然留到下一次解析 黑板分输入区 状态区 领域数据三区 状态区对所有系统与控制器开放读 相机在 TG_PostUpdateWork 跟随角色 不成为输入 Tick 的前置依赖

## 规则

应用顺序只在 `FBBBCharacterInputProcessor::Update` 中显式排列 数据包通过 `CanApply` 和 `Apply` 自描述条件与效果 不存在运行时排序 注册表 身份位或虚函数

基底输入每帧覆盖控制基座 本帧未提交时黑板保留上帧值 有效切换立即结束原换弹 空快捷槽不取消换弹 换弹优先于开火且换弹期间阻止开火 瞄准优先于冲刺 一次性失败请求不缓存 没有通用能力任务或效果框架

蒙太奇输入按五个 Slot 分为五种固定包 每个槽位保存空或一个蒙太奇 多轨蒙太奇共享播放修订号与一个值类型播放状态 沿用资产 Group 互斥 旧播放结束不能清掉新修订号

## 网络

本机客户端只上传输入请求 权威角色负责生成事实并同步全部远端副本 网络组件把事实转换为还原输入后通过统一入口投递 非权威角色不生成事实 `FBBBCharacterNetworkSystem` 只暴露一个 `Update` 位置和旋转继续由 CharacterMovement 同步

## 迁移与检查

见 [角色输入迁移说明](Character/Input/README.md) 本次不自动修改资产 不保留旧接口兼容层

编译目标为 ABBB_EvacEditor Win64 Development 不替代 PIE 或联机运行验证
