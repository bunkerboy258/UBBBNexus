#pragma once
class ABBBConsumable;
/** 消耗品公共生命周期 */
class FBBBConsumableShutdown
{
public:
    virtual ~FBBBConsumableShutdown() = default;
    /** @param Actor 当前消耗品 @return 生命周期处理结果 */
    void Shutdown(ABBBConsumable &Actor) const;
protected:
    /** @param Actor 完成公共处理的实例 @return 具体生命周期处理结果 */
    virtual void ShutdownInstance(ABBBConsumable &Actor) const = 0;
};
