#pragma once

#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "BBBCharacterPhysicalAnimationComponent.generated.h"

/** 在取得物理场景锁前完成角色骨骼任务的物理动画桥接 */
UCLASS(ClassGroup = "BBB", meta = (DisplayName = "角色物理动画"))
class ABBB_EVAC_API UBBBCharacterPhysicalAnimationComponent final : public UPhysicalAnimationComponent
{
    GENERATED_BODY()

public:
    /**
     * 先完成骨骼任务 再更新物理驱动
     * @param DeltaTime\t本帧间隔
     * @param TickType\t更新类型
     * @param ThisTickFunction\t当前更新函数
     * @return 无返回值
     */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
