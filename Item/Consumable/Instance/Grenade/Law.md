# Grenade Law

- 手持手雷负责使用 释放与消耗数量 不负责释放后的物理运动和爆炸
- 房主根据有效释放结果创建独立物理投掷物 Actor 使用 Chaos 与 Predictive Interpolation
- 投掷物由自身生命周期管理 手持实例失活或数量耗尽不得影响已释放的实体
