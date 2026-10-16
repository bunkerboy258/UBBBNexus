#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionState.h"

/** 只还原更新的网格显示结果 */
struct FBBBMeleeVisibilityAuthorityFactPacket final : public FBBBEquipmentVisibilityAuthorityFactPacket<FBBBMeleeVisibilityAuthorityFactPacket>
{
    /** @return 结果数组完整且版本有效 */
    bool IsValid() const { return !this->Hidden.IsEmpty() && this->Hidden.Num() == this->Revisions.Num() && !this->Revisions.Contains(uint64(0)); }
    /** @param State 当前事实 @return 允许核对结果版本 */
    bool CanApply(const FBBBMeleeActionState &State) const { return true; }
    /** @param State 本装备网格结果 @return 无 */
    void Apply(FBBBMeleeActionState &State) const
    {
        for (int32 Index = 0; Index < this->Revisions.Num(); ++Index)
        {
            if (this->Revisions[Index] > State.VisibilityRevision)
            {
                State.bHide = this->Hidden[Index];
                State.VisibilityRevision = this->Revisions[Index];
            }
        }
    }
};
