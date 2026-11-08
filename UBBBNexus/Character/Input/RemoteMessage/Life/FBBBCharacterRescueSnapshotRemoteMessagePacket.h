#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 救援离散输入 */
struct FBBBCharacterRescueSnapshotRemoteMessagePacket final
{
    /** 镜像结果伙伴 */
    TArray<TWeakObjectPtr<APawn>> SnapshotPartners;
    /** 镜像结果操作 */
    TArray<uint64> SnapshotOperations;
    /** 镜像结果倒地轮次 */
    TArray<uint64> SnapshotRounds;
    /** 镜像结果版本 */
    TArray<uint64> SnapshotRevisions;
    /** 镜像帮扶结果 */
    TArray<bool> SnapshotHelping;
    /** 镜像被救结果 */
    TArray<bool> SnapshotReceiving;

    /** 镜像当前请求已被接受 */
    TArray<bool> SnapshotAccepted;
    /** 镜像当前已进行时间 */
    TArray<float> SnapshotElapsed;
    /** 镜像时长 */
    TArray<float> SnapshotDurations;
    /** 镜像结束原因 */
    TArray<FName> SnapshotReasons;

    /** @return 数据是否完整 */
    bool IsValid() const
    {
        if (!(!SnapshotPartners.IsEmpty() && SnapshotOperations.Num() == SnapshotPartners.Num() && SnapshotRounds.Num() == SnapshotPartners.Num() && SnapshotRevisions.Num() == SnapshotPartners.Num() && SnapshotHelping.Num() == SnapshotPartners.Num() && SnapshotReceiving.Num() == SnapshotPartners.Num() && SnapshotAccepted.Num() == SnapshotPartners.Num() && SnapshotElapsed.Num() == SnapshotPartners.Num() && SnapshotDurations.Num() == SnapshotPartners.Num() && SnapshotReasons.Num() == SnapshotPartners.Num()))
        {
            return false;
        }
        for (int32 Index = 0; Index < SnapshotPartners.Num(); ++Index)
        {
            if (SnapshotRevisions[Index] == 0 || !FMath::IsFinite(SnapshotElapsed[Index]) ||
                SnapshotElapsed[Index] < 0.0f || !FMath::IsFinite(SnapshotDurations[Index]) ||
                SnapshotDurations[Index] <= 0.0f || (SnapshotHelping[Index] && SnapshotReceiving[Index]) ||
                (SnapshotReceiving[Index] && !SnapshotAccepted[Index]) ||
                (SnapshotAccepted[Index] && !SnapshotHelping[Index] && !SnapshotReceiving[Index]) ||
                ((SnapshotHelping[Index] || SnapshotReceiving[Index]) &&
                    (SnapshotOperations[Index] == 0 || SnapshotRounds[Index] == 0)))
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.RescueInputs.SnapshotPartners.Append(SnapshotPartners);
        Context.RescueInputs.SnapshotOperations.Append(SnapshotOperations);
        Context.RescueInputs.SnapshotRounds.Append(SnapshotRounds);
        Context.RescueInputs.SnapshotRevisions.Append(SnapshotRevisions);
        Context.RescueInputs.SnapshotHelping.Append(SnapshotHelping);
        Context.RescueInputs.SnapshotReceiving.Append(SnapshotReceiving);
        Context.RescueInputs.SnapshotAccepted.Append(SnapshotAccepted);
        Context.RescueInputs.SnapshotElapsed.Append(SnapshotElapsed);
        Context.RescueInputs.SnapshotDurations.Append(SnapshotDurations);
        Context.RescueInputs.SnapshotReasons.Append(SnapshotReasons);
    }
};
