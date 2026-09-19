# 角色输入

所有输入统一视为离散快照 每帧压入黑板快照区队列 不再区分持续与离散

外部只通过 `ABBBCharacter::SubmitInput(Packet)` 提交 闸口负责游戏线程检查与包自检

输入定义按语义拆分为独立文件 一种输入包一对 h cpp

```text
Input/
├── BBBCharacterInputSubmit            唯一提交闸口 线程检查与自检
├── BBBCharacterPacketRegistry         封闭包类型注册表与仲裁全景
└── Packets/
    ├── BBBApprovedPackets             本帧已批准集合
    ├── BBBCharacterPacketContext      包执行上下文
    ├── Base/                          移动与瞄准基底快照 每帧覆盖控制基座
    ├── Request/                       开火 换弹 切枪 跳跃请求
    ├── ReloadPhase/                   换弹卸下 装填 中断动画通知
    ├── Fact/                          装备已执行事实
    ├── Restore/                       网络还原装备 瞄准 步态
    └── Presentation/                  蒙太奇与相机表现
```

黑板 `FBBBCharacterRuntimeData` 分三个区域

| 区域 | 内容 | 权限 |
| --- | --- | --- |
| 快照区 | 本帧输入队列 `Snapshot` | 仅提交闸口与解析系统可触 帧内消费完毕 |
| 状态区 | 解析后包应用效果 Control Aim Locomotion Equipment Animation Camera 等 | 对所有系统与控制器开放读 |
| 领域数据 | 各系统与控制器私有状态 如 ParseState | 本系统内部 对外有限开放 |

每个包自描述三要素 `IsValid` 提交校验 `CanExecute` 执行条件 `Execute` 黑板效果 并声明编译期 `Priority` 与 `ApprovedBit` 身份位

包不持有规则对象 不注册委托 不直接调用任一控制器 由 `ParseSystem` 中的 `FBBBCharacterInputProcessor` 在主管线固定位置统一解析

## 仲裁模型

每帧按 `Priority` 稳定排序后分带处理

1. 还原带 100 保到达序执行网络还原
2. 事实带 50 保到达序执行装备已执行事实并驱动换弹状态机
3. 基底带 30 每帧覆盖控制基座 本帧未提交时黑板保留上帧值 还原模式由还原包直写对应域
4. 请求带执行两阶段 先按序对全部请求求值 `CanExecute` 再统一提交 `Execute` 冲突由失败方查询已批准集合单向声明 切枪 19 否决换弹 18 否决开火 17 跳跃 16 独立
5. 阶段带 15 换弹动画通知经序号守卫后转发装备命令
6. 表现带 5/4 蒙太奇经槽位守卫写入期望 相机追加贡献
7. 派生门控由 Finalize 统一处理 瞄准或开火时禁止冲刺

请求带铁律 `CanExecute` 只读解析状态与已批准集合 禁止读控制基座 否则两阶段失去意义

跨帧换弹状态机集中在 `FBBBCharacterParseState` 的语义方法中 包只调用 `Track` `Report` `Commit` `Apply` 接口 不直接触碰字段

## 新增一个输入包

1. 在对应带子目录新建一对 h cpp 声明 `Priority` 与 `ApprovedBit` 并实现三要素
2. 在 `BBBCharacterPacketRegistry` 的 Variant 与断言数组各登记一行
3. 提交方每帧或事件触发时调用 `SubmitInput`

## 手动资产迁移

1. 在玩家控制器的 `PlayerInputSystem` 配置 InputAction 引用
2. 保留 `FullBody` `UpperBody` `FullBodyAdditivePreAim` `UpperBodyAdditive` `AdditiveHitReact` 五个动画 Slot
3. 换弹通知继续使用 `BBB.Reload.Start` 和 `BBB.Reload.End` 并经 `ReportReloadStartNotify` 和 `ReportReloadEndNotify` 提交换弹阶段包
4. 不要同时添加自动通知和手工通知

包自检失败 槽位无效或蒙太奇播放失败会产生日志 镜像角色的玩法状态只由还原包写入
