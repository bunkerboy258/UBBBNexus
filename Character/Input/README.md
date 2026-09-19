# 角色输入

外部只通过 `ABBBCharacter::GetInput()` 提交持续状态与离散输入包

输入定义按语义拆分为独立文件 一个包一个文件

```text
Input/
├── States/
│   ├── BBBCharacterMovementState      移动 朝向和步态持续状态
│   ├── BBBCharacterAimState           瞄准持续状态
│   └── BBBCharacterInputStates        持续状态聚合
├── Packets/
│   ├── BBBApprovedPackets             本帧已批准集合
│   ├── BBBCharacterPacketContext      包执行上下文
│   ├── Request/                       开火 换弹 切枪 跳跃请求
│   ├── ReloadPhase/                   换弹卸下 装填 中断动画通知
│   ├── Fact/                          装备已执行事实
│   ├── Restore/                       网络还原装备 瞄准 步态
│   └── Presentation/                  蒙太奇与相机表现
└── BBBCharacterPacketRegistry         封闭包类型注册表与仲裁全景
```

| 通道 | 类型 | 生命周期 |
| --- | --- | --- |
| 持续状态 | `FBBBCharacterInputStates` 覆盖式提交 | 保留最后一份快照 |
| 离散包 | `FBBBCharacterPacket` 封闭 Variant 包类型 | 本帧消费一次 |

每个包自描述三要素 `IsValid` 提交校验 `CanExecute` 执行条件 `Execute` 黑板效果 并声明编译期 `Priority` 与 `ApprovedBit` 身份位

`FBBBCharacterInput` 只负责校验和收件 `ParseSystem` 中的 `FBBBCharacterInputProcessor` 在主管线的固定位置解析所有包 输入包不持有规则对象 不注册委托 不直接调用任一控制器

## 仲裁模型

每帧按 `Priority` 稳定排序后分带处理

1. 还原带 100 保到达序执行网络还原
2. 事实带 50 保到达序执行装备已执行事实并驱动换弹状态机
3. 请求带执行两阶段 先按序对全部请求求值 `CanExecute` 再统一提交 `Execute` 冲突由失败方查询已批准集合单向声明 切枪 19 否决换弹 18 否决开火 17 跳跃 16 独立
4. 阶段带 15 换弹动画通知经序号守卫后转发装备命令
5. 表现带 5/4 蒙太奇经槽位守卫写入期望 相机追加贡献
6. 派生门控由 Finalize 统一处理 瞄准或开火时禁止冲刺

跨帧换弹状态机集中在 `FBBBCharacterParseState` 的语义方法中 包只调用 `Track` 与 `Report` 接口 不直接触碰字段

## 手动资产迁移

1. 在玩家控制器的 `PlayerInputSystem` 配置 InputAction 引用
2. 保留 `FullBody` `UpperBody` `FullBodyAdditivePreAim` `UpperBodyAdditive` `AdditiveHitReact` 五个动画 Slot
3. 换弹通知继续使用 `BBB.Reload.Start` 和 `BBB.Reload.End` 并经 `ReportReloadStartNotify` 和 `ReportReloadEndNotify` 提交换弹阶段包
4. 不要同时添加自动通知和手工通知

包自检失败 槽位无效或蒙太奇播放失败会产生日志 镜像角色的玩法状态只由还原包写入
