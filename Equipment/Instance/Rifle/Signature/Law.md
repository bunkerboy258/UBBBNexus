# Rifle Signature Law

- Signature 是步枪全部行为规则的唯一实现位置，必须完整实现公共固定输入的全部 `Resolve/Restore` 重载。
- `Resolve` 负责本机规则检查、状态更新、玩法结果、表现和事实提交。
- `Restore` 只根据网络确认输入恢复状态与表现，禁止生成弹丸、后坐力或新的角色事实。
- 不支持的公共输入必须保留显式空实现，禁止隐藏协议缺口或转入运行时分派。
- Signature 禁止声明私有输入队列、额外 Tick、行为 UObject 或处理器链。
