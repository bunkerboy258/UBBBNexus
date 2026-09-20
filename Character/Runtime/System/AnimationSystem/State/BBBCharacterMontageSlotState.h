#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Request/BBBCharacterMontageRequest.h"
#include "BBBCharacterMontageSlotState.generated.h"

/** 固定槽位期望状态，多槽蒙太奇通过相同修订号共享一次播放 */
USTRUCT()
struct FBBBCharacterMontageSlotState
{
    GENERATED_BODY()

    UPROPERTY()
    FName Slot = NAME_None;

    UPROPERTY()
    FBBBCharacterMontagePacket Desired;

    uint64 Revision = 0;
};

/**
 * 角色动画图支持的五个固定蒙太奇槽位
 *
 * 显式成员既允许UObject引用被反射追踪，也让Rider审核时无需推断数组索引含义
 */
USTRUCT()
struct FBBBCharacterMontageSlots
{
    GENERATED_BODY()

    FBBBCharacterMontageSlots()
    {
        FullBody.Slot = TEXT("FullBody");
        UpperBody.Slot = TEXT("UpperBody");
        FullBodyAdditivePreAim.Slot = TEXT("FullBodyAdditivePreAim");
        UpperBodyAdditive.Slot = TEXT("UpperBodyAdditive");
        AdditiveHitReact.Slot = TEXT("AdditiveHitReact");
    }

    /**
     * 查找明确命名的固定槽位
     * @param Slot  动画槽位名
     * @return 对应槽位状态，名称不受支持时返回空
     */
    FBBBCharacterMontageSlotState *Find(FName Slot)
    {
        if (FullBody.Slot == Slot)
        {
            return &FullBody;
        }

        if (UpperBody.Slot == Slot)
        {
            return &UpperBody;
        }

        if (FullBodyAdditivePreAim.Slot == Slot)
        {
            return &FullBodyAdditivePreAim;
        }

        if (UpperBodyAdditive.Slot == Slot)
        {
            return &UpperBodyAdditive;
        }

        if (AdditiveHitReact.Slot == Slot)
        {
            return &AdditiveHitReact;
        }

        return nullptr;
    }

    /**
     * 只读查找明确命名的固定槽位
     * @param Slot  动画槽位名
     * @return 对应槽位状态，名称不受支持时返回空
     */
    const FBBBCharacterMontageSlotState *Find(FName Slot) const
    {
        if (FullBody.Slot == Slot)
        {
            return &FullBody;
        }

        if (UpperBody.Slot == Slot)
        {
            return &UpperBody;
        }

        if (FullBodyAdditivePreAim.Slot == Slot)
        {
            return &FullBodyAdditivePreAim;
        }

        if (UpperBodyAdditive.Slot == Slot)
        {
            return &UpperBodyAdditive;
        }

        if (AdditiveHitReact.Slot == Slot)
        {
            return &AdditiveHitReact;
        }

        return nullptr;
    }

    /**
     * 按固定层级访问全部槽位
     * @param Function  接收可写槽位引用的函数对象
     */
    template<typename TFunction>
    void ForEach(TFunction &&Function)
    {
        Function(FullBody);
        Function(UpperBody);
        Function(FullBodyAdditivePreAim);
        Function(UpperBodyAdditive);
        Function(AdditiveHitReact);
    }

    /**
     * 按固定层级只读访问全部槽位
     * @param Function  接收只读槽位引用的函数对象
     */
    template<typename TFunction>
    void ForEach(TFunction &&Function) const
    {
        Function(FullBody);
        Function(UpperBody);
        Function(FullBodyAdditivePreAim);
        Function(UpperBodyAdditive);
        Function(AdditiveHitReact);
    }

    /**
     * 查询播放修订号是否仍被任一槽位期望
     * @param Revision  播放修订号
     * @return 是否仍存在对应期望
     */
    bool ContainsRevision(const uint64 Revision) const
    {
        bool bFound = false;
        ForEach([Revision, &bFound](const FBBBCharacterMontageSlotState &Slot)
        {
            bFound |= Slot.Revision == Revision;
        });
        return bFound;
    }

    UPROPERTY(Transient)
    FBBBCharacterMontageSlotState FullBody;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlotState UpperBody;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlotState FullBodyAdditivePreAim;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlotState UpperBodyAdditive;

    UPROPERTY(Transient)
    FBBBCharacterMontageSlotState AdditiveHitReact;
};
