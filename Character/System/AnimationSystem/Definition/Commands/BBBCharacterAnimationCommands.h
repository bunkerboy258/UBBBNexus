
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAnimationCommands.generated.h"
class FBBBCharacterExternalAPI;
struct FBBBAnimationRuntimeData;
class UAnimMontage;

USTRUCT(BlueprintType)
//动画请求数据
struct FBBBCharacterAnimationRequest
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UAnimMontage> Montage = nullptr;

    UPROPERTY()
    //是否允许打断式重复播放
    bool bRestartIfPlaying = false;
};

USTRUCT(BlueprintType)
//角色的动画请求容器
struct FBBBCharacterAnimationCommands
{
    GENERATED_BODY()

    /**
     * 本帧是否请求屏蔽物品IK
     * @return 请求屏蔽物品IK时返回true
     */
    bool IsEquipmentIKBlockedRequested() const
    {
        return bBlockEquipmentIK;
    }

    /**
     * 读取本帧待播放的蒙太奇请求队列
     * @return 待播放蒙太奇请求队列
     */
    const TArray<FBBBCharacterAnimationRequest> &GetPendingMontages() const
    {
        return PendingMontages;
    }
private:
    friend class FBBBCharacterExternalAPI;
    friend struct FBBBAnimationRuntimeData;

    /**
     * 将蒙太奇请求加入待播放队列
     * @param Request	蒙太奇播放请求
     */
    void QueueMontage(const FBBBCharacterAnimationRequest &Request)
    {
        PendingMontages.Add(Request);
    }

    /**
     * 提交本帧物品IK屏蔽请求
     * @param bBlocked	是否屏蔽物品IK
     */
    void SubmitEquipmentIKBlockRequest(bool bBlocked)
    {
        bBlockEquipmentIK = bBlocked;
    }

    /**
     * 清理本帧蒙太奇队列与物品IK屏蔽请求
     */
    void CleanFrame()
    {
        PendingMontages.Reset();
        bBlockEquipmentIK = false;
    }

    UPROPERTY()
    TArray<FBBBCharacterAnimationRequest> PendingMontages;

    UPROPERTY()
    bool bBlockEquipmentIK = false;
};
