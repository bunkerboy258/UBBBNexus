#pragma once
class UBBBConsumable;
/** 消耗品公共生命周期 */
class FBBBConsumableShutdown
{
public:
    virtual ~FBBBConsumableShutdown() = default;
    /** @param Item 当次使用对象 @return 生命周期处理结果 */
    void Shutdown(UBBBConsumable &Item) const;

  protected:
    /** @param Item 当次使用对象 @return 具体实例处理结果 */
    virtual void ShutdownInstance(UBBBConsumable &Item) const = 0;
};
