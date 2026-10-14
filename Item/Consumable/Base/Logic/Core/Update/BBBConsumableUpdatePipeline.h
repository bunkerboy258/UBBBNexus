#pragma once
class ABBBConsumable;
/** 消耗品公共生命周期 */
class FBBBConsumableUpdatePipeline
{
public:
    /** @param Actor 当前消耗品 @return 无 配置固定 Tick 调度属性 */
    static void ConfigureTick(ABBBConsumable &Actor);
    virtual ~FBBBConsumableUpdatePipeline() = default;
    /** @param Actor 当前消耗品 @return 生命周期处理结果 */
    void Update(ABBBConsumable &Actor) const;
protected:
    /** @param Actor 完成公共处理的实例 @return 具体生命周期处理结果 */
    virtual void UpdateInstance(ABBBConsumable &Actor) const = 0;
};
