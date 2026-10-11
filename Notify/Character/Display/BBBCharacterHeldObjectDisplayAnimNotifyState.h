#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterHeldObjectDisplayAnimNotifyState.generated.h"

class UStaticMesh;

/** 在角色动作指定区间附着临时手持物体 只改变表现 不改变物品或弹药状态 */
UCLASS(meta = (DisplayName = "BBB 角色手持物体表现"))
class ABBB_EVAC_API UBBBCharacterHeldObjectDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 临时表现网格 不产生碰撞或玩法实例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|角色|手持表现", meta = (DisplayName = "物体网格", ToolTip = "仅在通知区间显示的静态网格 不改变物品或弹药状态"))
    TObjectPtr<UStaticMesh> ObjectMesh = nullptr;

    /** 直接播放通知的角色网格上的挂接骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|角色|手持表现", meta = (DisplayName = "挂接骨骼", AnimNotifyBoneName = "true"))
    FName HandBoneName = TEXT("hand_l");

    /** 相对挂接骨骼的变换 位置单位为厘米 旋转单位为度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|角色|手持表现", meta = (DisplayName = "手持变换", ToolTip = "物体相对挂接骨骼的变换 位置单位为厘米 旋转单位为度"))
    FTransform HandObjectTransform = FTransform::Identity;

    /**
     * 在直接播放通知的网格上附着临时表现组件
     * @param MeshComp		角色动画网格
     * @param Animation		动画资产
     * @param TotalDuration		通知区间秒数
     * @param EventReference		通知事件引用
     * @return 无
     */
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        float TotalDuration, const FAnimNotifyEventReference &EventReference) override;

    /**
     * 在区间结束或动作中断时销毁本通知创建的表现组件
     * @param MeshComp		角色动画网格
     * @param Animation		动画资产
     * @param EventReference		通知事件引用
     * @return 无
     */
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        const FAnimNotifyEventReference &EventReference) override;
};
