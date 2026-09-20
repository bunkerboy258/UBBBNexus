# Rifle Law

- `FBBBRifleSignature::Resolve/Restore` 是步枪行为的唯一实现入口。
- `FBBBRifleState` 保存全部跨帧步枪事实。
- `Resolve` 可以判断规则并产生新事实；`Restore` 只能还原网络确认事实和表现。
- 步枪禁止声明私有输入包，必须完整实现 `Template/Input` 的公共固定输入协议。
