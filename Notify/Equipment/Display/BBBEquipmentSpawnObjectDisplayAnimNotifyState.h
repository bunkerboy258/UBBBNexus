#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBEquipmentSpawnObjectDisplayAnimNotifyState.generated.h"

class UStaticMesh;

/** 装备动画在通知开始时生成一次带物理速度的视觉物体 */
UCLASS(meta = (DisplayName = "BBB 装备生成物体"))
class ABBB_EVAC_API UBBBEquipmentSpawnObjectDisplayAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    /** 生成的静态网格 需要能够进行简单碰撞物理模拟 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|物体", meta = (DisplayName = "物体网格", ToolTip = "用于弹壳等独立视觉物体 网格须具备简单碰撞 留空时跳过"))
    TObjectPtr<UStaticMesh> ObjectMesh = nullptr;

    /** 物体生成使用的装备插槽或骨骼 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|物体", meta = (DisplayName = "生成插槽", AnimNotifyBoneName = "true", ToolTip = "使用当前播放动画的装备网格插槽 留空时使用网格原点"))
    FName SocketName = NAME_None;

    /** 相对于生成插槽的物体变换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|物体", meta = (DisplayName = "相对变换", ToolTip = "控制物体的初始位置 朝向和尺寸 缩放各轴必须大于零"))
    FTransform RelativeTransform = FTransform::Identity;

    /** 插槽局部坐标中的初始线速度 单位厘米每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|物体", meta = (DisplayName = "初始线速度", ToolTip = "单位厘米每秒 方向使用生成插槽的局部坐标 不受物体缩放影响"))
    FVector LinearVelocity = FVector::ZeroVector;

    /** 插槽局部坐标中的初始旋转速度 单位度每秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|物体", meta = (DisplayName = "初始旋转速度", ToolTip = "单位度每秒 三个分量分别绕生成插槽的 X Y Z 轴旋转"))
    FVector AngularVelocityDegrees = FVector::ZeroVector;

    /** 物体生成后自动销毁的时间 单位秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|装备|物体", meta = (ClampMin = "0.01", DisplayName = "存在时间", ToolTip = "单位秒 到时自动销毁 不承担玩法或网络同步"))
    float LifeSeconds = 3.0f;

    /**
     * 在通知开始时生成一次物体并赋予速度
     * @param MeshComp		播放装备动画的网格
     * @param Animation		动画资产
     * @param TotalDuration	通知区间长度
     * @param EventReference	通知事件引用
     * @return 无
     */
    virtual void NotifyBegin(
        USkeletalMeshComponent *MeshComp,
        UAnimSequenceBase *Animation,
        float TotalDuration,
        const FAnimNotifyEventReference &EventReference) override;
};
