#pragma once
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Network/BBBConsumableNetworkMessage.h"
#include "BBBConsumableNetworkComponent.generated.h"

/** 持有角色上的消耗品消息载体 不执行使用规则 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBConsumableNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    UBBBConsumableNetworkComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
    /** @param Message	已成立的当前结果 @return 无 */
    void Publish(FBBBConsumableNetworkMessage Message);
    /** @return 无 向当前有效实例投递结果 */
    void DeliverPending();
private:
    /** @param Message	控制端已成立结果 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitState(FBBBConsumableNetworkMessage Message);
    /** @return 无 通过实例输入投递当前结果 */
    UFUNCTION()
    void OnRep_State();
    /** 最新结果传输副本 */
    UPROPERTY(ReplicatedUsing = OnRep_State)
    FBBBConsumableNetworkMessage State;
    /** 等待当前持有关系建立的最新结果 */
    FBBBConsumableNetworkMessage Pending;
    /** 已向实例投递的代次 */
    uint64 DeliveredGeneration = 0;
    /** 已向实例投递的顺序 */
    uint64 DeliveredRevision = 0;
};
