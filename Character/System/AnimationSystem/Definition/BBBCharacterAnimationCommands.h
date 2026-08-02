
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

    bool IsItemIKBlockedRequested() const
    {
        return bBlockItemIK;
    }

    const TArray<FBBBCharacterAnimationRequest> &GetPendingMontages() const
    {
        return PendingMontages;
    }
private:
    friend class FBBBCharacterExternalAPI;
    friend struct FBBBAnimationRuntimeData;

    void QueueMontage(const FBBBCharacterAnimationRequest &Request)
    {
        PendingMontages.Add(Request);
    }

    void SubmitItemIKBlockRequest(bool bBlocked)
    {
        bBlockItemIK = bBlocked;
    }

    void CleanFrame()
    {
        PendingMontages.Reset();
        bBlockItemIK = false;
    }

    UPROPERTY()
    TArray<FBBBCharacterAnimationRequest> PendingMontages;

    UPROPERTY()
    bool bBlockItemIK = false;
};
