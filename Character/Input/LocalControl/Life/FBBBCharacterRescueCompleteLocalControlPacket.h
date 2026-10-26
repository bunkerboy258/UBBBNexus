#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 接受救援动作完成的通知输入 只贡献本次操作的完成请求 */
struct FBBBCharacterRescueCompleteLocalControlPacket final
{
    /** 通知开始时绑定的救援操作 */
    uint64 OperationId = 0;

    /** 通知开始时绑定的倒地轮次 */
    uint64 DownedRevision = 0;

    /** @return 操作与倒地轮次是否有效 */
    bool IsValid() const
    {
        return OperationId != 0 && DownedRevision != 0;
    }

    /** @param Context\t本次输入上下文 @return 是否仍属于当前接受中的救援 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.Phase == EBBBCharacterLifePhase::Downed
            && Context.Rescue.bReceiving && Context.Rescue.bAccepted
            && Context.Rescue.OperationId == OperationId
            && Context.Rescue.DownedRevision == DownedRevision
            && Context.Life.DownedRevision == DownedRevision;
    }

    /** @param Context\t本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.RescueInputs.CompletionOperations.Add(OperationId);
        Context.RescueInputs.CompletionRounds.Add(DownedRevision);
    }
};
