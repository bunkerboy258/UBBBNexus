# Throwable Law

- 本目录定义消耗品释放后的独立世界物理投掷物 不定义手持使用与背包数量
- 遵循 Actor 数据 网络和注释宪法 房主为唯一业务控制端
- 使用 UE Chaos 和 Predictive Interpolation 不自写弹道积分 不逐帧复制自定义变换
- 生命周期与释放来源独立 物理状态由引擎持有 业务状态经唯一 RuntimeData 持有
- 房主产生一次爆炸事实 各端表现一次 投掷者按确定结果一次性记入已有伤害累计入口
- 配置按发射 物理 爆炸 表现划分 每个配置定义独占文件 字段使用引擎内可见中文说明

## 物理目录

```text
Projectile/
    BBBThrowable.h / .cpp
    Config/
        Launch/
        Physics/
        Explosion/
        Presentation/
    Input/
        LocalControl/
            Lifecycle/
            Explosion/
        AuthorityFact/
            Explosion/
    Network/
    Logic/
        Core/
            Initialization/
            Update/
            Shutdown/
        RuntimeData/
        System/
            ParseSystem/
            LifetimeSystem/
            ExplosionSystem/
            NetworkSystem/
```
