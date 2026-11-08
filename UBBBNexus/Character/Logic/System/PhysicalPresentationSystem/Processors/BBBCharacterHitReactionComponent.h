#pragma once
#include "CoreMinimal.h"
#include "HitReact.h"
#include "BBBCharacterHitReactionComponent.generated.h"

class UPrimitiveComponent;
class USkeletalMeshComponent;

/** 仅提供插件模拟的完整清理 不持有角色生命规则 */
UCLASS(ClassGroup = "BBB", meta = (DisplayName = "角色物理受击"))
class ABBB_EVAC_API UBBBCharacterHitReactionComponent final : public UHitReact
{
    GENERATED_BODY()
  private:
    friend class FBBBCharacterPhysicalPresentationProcessor;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterShutdown;
    /** @param Body 身体网格 @param bBind 是否绑定 @return 无 */
    void BindBodyContacts(USkeletalMeshComponent &Body, bool bBind);
    /** @param HitComponent 身体 @param OtherActor 对象 @param OtherComponent 对象组件 @param NormalImpulse 冲量 @param Hit 接触事实 @return 无 */
    UFUNCTION()
    void OnBodyContact(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent,
        FVector NormalImpulse, const FHitResult &Hit);
    /** @return 无 清除局部模拟并停止更新 */
    void ClearSimulation();
};
