
#pragma once
#include "CoreMinimal.h"
#include "BBBIntentRuntimeData.generated.h"
struct FBBBCharacterIntentState;

USTRUCT(BlueprintType)
//当前帧的行为意图
struct FBBBIntentRuntimeData
{
    GENERATED_BODY()

    const FVector2D &GetMoveInput() const
    {
        return MoveInput;
    }

    const FVector2D &GetSmoothedMoveInput() const
    {
        return SmoothedMoveInput;
    }

    bool HasMoveInput() const
    {
        return bHasMoveInput;
    }

    bool WantsFire() const
    {
        return bWantsFire;
    }

    bool WantsAim() const
    {
        return bWantsAim;
    }

    bool WantsSprint() const
    {
        return bWantsSprint;
    }

    bool WantsReload() const
    {
        return bWantsReload;
    }

    int32 GetRequestedEquipSlot() const
    {
        return RequestedEquipSlot;
    }

    void CommitFrame(const FBBBIntentRuntimeData &IntentData)
    {
        *this = IntentData;
    }

    //清除一次性意图 并保留移动平滑连续性
    void CleanFrame()
    {
        //跨帧保留平滑值
        const FVector2D PersistentSmoothedMoveInput = SmoothedMoveInput;
        
        *this = FBBBIntentRuntimeData();
        
        SmoothedMoveInput = PersistentSmoothedMoveInput;
    }
private:
    
    friend struct FBBBCharacterIntentState;

    //即时二维输入
    UPROPERTY()
    FVector2D MoveInput = FVector2D::ZeroVector;

    //跨帧平滑输入
    UPROPERTY()
    FVector2D SmoothedMoveInput = FVector2D::ZeroVector;

    //是否应视为存在移动输入
    UPROPERTY()
    bool bHasMoveInput = false;

    //持续开火请求
    UPROPERTY()
    bool bWantsFire = false;

    //瞄准姿态请求
    UPROPERTY()
    bool bWantsAim = false;

    //冲刺速度请求
    UPROPERTY()
    bool bWantsSprint = false;

    //换弹请求
    UPROPERTY()
    bool bWantsReload = false;

    //装备槽位请求
    UPROPERTY()
    int32 RequestedEquipSlot = INDEX_NONE;
};
