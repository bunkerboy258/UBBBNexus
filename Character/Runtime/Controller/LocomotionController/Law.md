# Locomotion Controller Law

- 本控制器只把解析后的控制事实应用到角色与 CMC，并形成最终移动状态。
- 只有 `Causal` 角色允许执行 `Update`；Mirror 角色只接受网络还原后的最终移动状态。
- 所有 CMC 写入必须集中在 Locomotion Processor，禁止其它角色领域直接产生移动输入。
- 本领域禁止拥有网络发送、动画播放和装备行为；动画只在 CMC 完成后读取最终结果。
