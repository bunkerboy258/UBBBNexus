#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/DomainData/States/BBBWorldDropInputState.h"
/** 只按固定顺序解析掉落物输入槽位 */
class FBBBWorldDropInputProcessor final
{
public:
    /** @param Slots\t固定输入槽位 @param Context\t本次解析依赖 @return 无 */
    void Update(FBBBWorldDropInputState &Slots, FBBBWorldDropInputContext &Context) const;
private:
    /** @param Slot\t一种输入的固定槽 @param Context\t本次解析依赖 @return 无 */
    template<typename TPacket>
    static void Process(TBBBWorldDropInputSlot<TPacket> &Slot, FBBBWorldDropInputContext &Context)
    {
        if (!Slot.bActive)
        {
            return;
        }
        if (Slot.Data.IsValid() && Slot.Data.CanApply(Context))
        {
            Slot.Data.Apply(Context);
        }
        Slot.Data = {};
        Slot.bActive = false;
    }
};
