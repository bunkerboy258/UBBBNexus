#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BBBCharacterMagazineMotionComponent.generated.h"

/** 保存单次角色手持弹匣的运动样本 通知资产不持有播放状态 */
UCLASS(Transient, NotBlueprintable)
class ABBB_EVAC_API UBBBCharacterMagazineMotionComponent final : public UStaticMeshComponent
{
    GENERATED_BODY()

public:
    /** 初始化角色姿势完成后的采样更新 */
    UBBBCharacterMagazineMotionComponent();

    /**
     * 读取当前附着世界变换并更新线速度与角速度
     * @return 无
     */
    void SampleMotion();

    /** @return 弹匣实测世界线速度 单位厘米每秒 */
    FVector GetReleaseLinearVelocity() const
    {
        return LinearVelocity;
    }

    /** @return 弹匣实测世界角速度 单位弧度每秒 */
    FVector GetReleaseAngularVelocity() const
    {
        return AngularVelocity;
    }

    /**
     * 在角色姿势完成后采集手持运动
     * @param DeltaTime		本帧时间
     * @param TickType		更新类型
     * @param ThisTickFunction	当前更新函数
     * @return 无
     */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
    /** 上一次采样的世界变换 */
    FTransform PreviousTransform = FTransform::Identity;

    /** 上一次采样的游戏时间 负值表示尚未取得首帧 */
    double PreviousTime = -1.0;

    /** 已包含角色移动的世界线速度 */
    FVector LinearVelocity = FVector::ZeroVector;

    /** 最短旋转弧对应的世界角速度 */
    FVector AngularVelocity = FVector::ZeroVector;
};
