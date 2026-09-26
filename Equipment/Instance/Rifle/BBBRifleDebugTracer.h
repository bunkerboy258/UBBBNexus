#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBRifleDebugTracer.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

/** PIE 中用于观察步枪开火节奏和枪口方向的纯视觉示踪弹体 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBRifleDebugTracer final : public AActor
{
    GENERATED_BODY()

public:
    /** 创建无碰撞的可视示踪弹体 @return 无 */
    ABBBRifleDebugTracer();

protected:
    /** 限制示踪弹体只在 PIE 中显示 */
    virtual void BeginPlay() override;

private:
    /** 发光示踪弹体网格 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|Rifle|Debug")
    TObjectPtr<UStaticMeshComponent> TracerMesh;

    /** 负责沿枪口前向移动的组件 */
    UPROPERTY(VisibleAnywhere, Category = "BBB|Rifle|Debug")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
};
