#pragma once
#include "Components/ActorComponent.h"
#include "BBBMonsterDirectorReplicationState.h"
#include "BBBMonsterDirectorNetworkComponent.generated.h"

/** 导演当前结果的 UE 属性复制桥接 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    /** @return 构造无逻辑 Tick 的复制组件 */
    UBBBMonsterDirectorNetworkComponent();
    /**
     * @param State	主机处理器已成立的当前结果
     * @return 无返回值
     */
    void Publish(const FBBBMonsterDirectorReplicationState& State);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    /** 对外传输结果副本 不生成玩法事实 */
    UPROPERTY(ReplicatedUsing = OnRep_State)
    FBBBMonsterDirectorReplicationState State;
    UFUNCTION()
    void OnRep_State();
};
