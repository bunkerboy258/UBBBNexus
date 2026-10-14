# Grenade Config Law

- 本目录只定义手雷的静态配置，配置根继承 `UBBBConsumableDefinition` 并声明为 `final`。
- 手雷配置只保存手雷专属参数并引用对应的 `UBBBProjectileDefinition`；投掷物运动、碰撞、引信与伤害配置由该定义持有。
- 不在此目录保存运行时状态或实现玩法逻辑。

## 物理目录

```text
Config/
├─ Law.md
└─ BBBGrenadeDefinition.h
```
