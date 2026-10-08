#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色外观的独立输入 */
struct FBBBCharacterAppearanceRemoteMessagePacket final
{
    /** 按到达顺序提交的完整既成事实 */
    TArray<FBBBCharacterAppearanceSnapshot> Snapshots;
    /** @return 输入结构是否有效 */
    bool IsValid() const
    {
        if (Snapshots.IsEmpty())
        {
            return false;
        }
        for (const FBBBCharacterAppearanceSnapshot &Snapshot : Snapshots)
        {
            if (!Snapshot.IsValid())
            {
                return false;
            }
        }
        return true;
    }
    /** @param Context 角色输入上下文 @return 是否允许本路径消费 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror;
    }
    /** @param Context 角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.AppearanceInputs.PendingSelections.Append(Snapshots);
    }
};
