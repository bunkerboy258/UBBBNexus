#pragma once
class UBBBConsumable;
/** 消耗品公共生命周期 */
class FBBBConsumableInitializer
{
public:
    virtual ~FBBBConsumableInitializer() = default;
    /** @param Item 当次使用对象 @return 生命周期处理结果 */
    bool Initialize(UBBBConsumable &Item) const;

  protected:
    /** @param Item 当次使用对象 @return 具体实例处理结果 */
    virtual bool InitializeInstance(UBBBConsumable &Item) const = 0;
};
