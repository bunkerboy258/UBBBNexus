#pragma once
class ABBBConsumable;
/** 消耗品公共生命周期 */
class FBBBConsumableInitializer
{
public:
    virtual ~FBBBConsumableInitializer() = default;
    /** @param Actor 当前消耗品 @return 生命周期处理结果 */
    bool Initialize(ABBBConsumable &Actor) const;
protected:
    /** @param Actor 完成公共处理的实例 @return 具体生命周期处理结果 */
    virtual bool InitializeInstance(ABBBConsumable &Actor) const = 0;
};
