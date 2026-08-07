
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"

//收集各意图处理器在当前帧写入的中间结果
struct FBBBCharacterIntentState
{
    //未经平滑的当前移动方向与强度
    FVector2D MoveInput = FVector2D::ZeroVector;

    //包含移动闪断缓冲后的移动有效标记
    bool bHasMoveInput = false;

    //当前帧是否请求武器开火
    bool bWantsFire = false;

    //当前帧是否需要进入瞄准姿态
    bool bWantsAim = false;

    //当前帧是否请求冲刺速度
    bool bWantsSprint = false;

    /** 当前帧是否请求角色跳跃 */
    bool bWantsJump = false;

    //当前帧是否请求武器换弹
    bool bWantsReload = false;

    //当前帧请求切换的库存槽位
    int32 RequestedEquipSlot = INDEX_NONE;

    /**
     * 将中间意图状态拷贝为运行时意图数据
     * @return 完整意图运行时数据
     */
    FBBBIntentRuntimeData ToRuntimeData() const
    {
        FBBBIntentRuntimeData RuntimeData;

        RuntimeData.MoveInput = MoveInput;
        RuntimeData.bHasMoveInput = bHasMoveInput;
        RuntimeData.bWantsFire = bWantsFire;
        RuntimeData.bWantsAim = bWantsAim;
        RuntimeData.bWantsSprint = bWantsSprint;
        RuntimeData.bWantsJump = bWantsJump;
        RuntimeData.bWantsReload = bWantsReload;
        RuntimeData.RequestedEquipSlot = RequestedEquipSlot;
        
        return RuntimeData;
    }
};
