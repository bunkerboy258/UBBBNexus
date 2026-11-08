#pragma once

#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "BBBMonsterPhysicalAnimationComponent.generated.h"

/** 在取得物理场景锁前完成小怪骨骼任务的物理动画桥接 */
UCLASS(ClassGroup = "Monster", meta = (DisplayName = "小怪物理动画"))
class ABBB_EVAC_API UBBBMonsterPhysicalAnimationComponent final : public UPhysicalAnimationComponent
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
