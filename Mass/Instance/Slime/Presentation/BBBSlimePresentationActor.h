#pragma once

#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFact.h"
#include "BBBSlimePresentationActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UNiagaraComponent;
class UDecalComponent;
class UMaterialInstanceDynamic;
class UBBBSlimeDefinition;

/** 粘液的无碰撞机械显示载体 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBSlimePresentationActor final : public AActor
{
    GENERATED_BODY()
public:
    ABBBSlimePresentationActor();
    /** @param Fact 当前飞行或地面结果 @return 只更新机械显示 */
    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void ApplyFact(const FBBBSlimeFact& Fact);
private:
    /** 配置变化时装配共享资源 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBSlimeDefinition> Definition;
    UPROPERTY()
    TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> Body;
    UPROPERTY()
    TObjectPtr<UNiagaraComponent> Trail;
    UPROPERTY()
    TObjectPtr<UNiagaraComponent> Bubbles;
    UPROPERTY()
    TObjectPtr<UDecalComponent> Puddle;
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> PuddleMaterial;
    /** 只防止重复播放瞬间效果 不保存命中过程 */
    uint8 AppliedPhase = 255;
    /** @param Fact 当前完整结果 @return 已装配有效的表现资源 */
    bool Configure(const FBBBSlimeFact& Fact);
};
