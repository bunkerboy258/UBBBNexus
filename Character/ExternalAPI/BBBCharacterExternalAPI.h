#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentActionType.h"

class UAnimMontage;
struct FBBBAnimationRuntimeData;

/** 装备向角色提交本地表现请求的唯一入口 */
class ABBB_EVAC_API FBBBCharacterExternalAPI final
{
public:
    /**
     * 注入角色动画运行数据
     * @param InAnimationData    角色动画数据根
     * @return 无
     */
    void Initialize(FBBBAnimationRuntimeData &InAnimationData);

    /**
     * 提交一条武器决定的蒙太奇贡献
     * @param ActionType         装备操作类型
     * @param Montage            人物动作蒙太奇
     * @param PlayRate           播放倍率
     * @return 是否成功入队
     */
    bool SubmitEquipmentMontage(
        EBBBEquipmentActionType ActionType,
        UAnimMontage *Montage,
        float PlayRate);

private:
    FBBBAnimationRuntimeData *AnimationData = nullptr;
};
