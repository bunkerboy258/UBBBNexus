
#pragma once
#include "CoreMinimal.h"
#include "BBBIntentRuntimeData.generated.h"
struct FBBBCharacterIntentState;

USTRUCT(BlueprintType)
//当前帧的行为意图
struct FBBBIntentRuntimeData
{
    GENERATED_BODY()

    /**
     * 读取即时二维移动输入
     * @return 移动输入二维值
     */
    const FVector2D &GetMoveInput() const
    {
        return MoveInput;
    }

    /**
     * 判断是否应视为存在移动输入
     * @return 是否存在移动输入
     */
    bool HasMoveInput() const
    {
        return bHasMoveInput;
    }

    /**
     * 判断当前帧是否请求持续开火
     * @return 是否请求开火
     */
    bool WantsFire() const
    {
        return bWantsFire;
    }

    /**
     * 判断当前帧是否请求瞄准姿态
     * @return 是否请求瞄准
     */
    bool WantsAim() const
    {
        return bWantsAim;
    }

    /**
     * 判断当前帧是否请求冲刺速度
     * @return 是否请求冲刺
     */
    bool WantsSprint() const
    {
        return bWantsSprint;
    }

    /** @return 当前帧是否请求角色保持蹲伏 */
    bool WantsCrouch() const
    {
        return bWantsCrouch;
    }

    /** @return 当前帧是否请求角色跳跃 */
    bool WantsJump() const
    {
        return bWantsJump;
    }

    /**
     * 判断当前帧是否请求换弹
     * @return 是否请求换弹
     */
    bool WantsReload() const
    {
        return bWantsReload;
    }

    /**
     * 读取当前帧请求的装备槽位
     * @return 装备槽位下标
     */
    int32 GetRequestedEquipSlot() const
    {
        return RequestedEquipSlot;
    }

    /**
     * 发布完整意图快照覆盖当前数据
     * @param IntentData	本帧意图快照
     */
    void CommitFrame(const FBBBIntentRuntimeData &IntentData)
    {
        *this = IntentData;
    }

    /**
     * 清除本帧意图
     */
    void CleanFrame()
    {
        *this = FBBBIntentRuntimeData();
    }
private:
    
    friend struct FBBBCharacterIntentState;

    //即时二维输入
    UPROPERTY()
    FVector2D MoveInput = FVector2D::ZeroVector;

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

    /** 蹲伏姿态请求 */
    UPROPERTY()
    bool bWantsCrouch = false;

    /** 跳跃请求 */
    UPROPERTY()
    bool bWantsJump = false;

    //换弹请求
    UPROPERTY()
    bool bWantsReload = false;

    //装备槽位请求
    UPROPERTY()
    int32 RequestedEquipSlot = INDEX_NONE;
};
