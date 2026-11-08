#pragma once
struct FBBBCharacterItemUpdateContext;
/** 唯一真实物品的最终回收 */
class FBBBCharacterItemLifecycleProcessor final
{
public:
    /** @param Context	本次物品上下文 @return 无 */
    void Update(FBBBCharacterItemUpdateContext &Context) const;
    /** @param Context	本次关闭上下文 @return 无 */
    void Shutdown(FBBBCharacterItemUpdateContext &Context) const;
};
