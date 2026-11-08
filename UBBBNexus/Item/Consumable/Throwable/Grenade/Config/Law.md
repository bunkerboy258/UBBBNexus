# Grenade Config Law

- 本目录只定义手雷的静态配置 配置根继承 UBBBConsumableDefinition 并声明为 final
- 手雷配置保存高抛与低抛的使用动作和释放参数 并引用 UBBBThrowableDefinition
- 物理投掷物的发射 碰撞 引信 爆炸与表现由 UBBBThrowableDefinition 持有
- 不在此目录保存运行时状态或实现玩法逻辑

## 物理目录

```text
Config/
    BBBGrenadeDefinition.h
```
