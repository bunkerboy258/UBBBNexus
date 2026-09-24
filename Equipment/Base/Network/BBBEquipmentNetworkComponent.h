#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkState.h"
#include "BBBEquipmentNetworkComponent.generated.h"

class ABBBEquipment;

/** 借用持有角色连接传输当前装备状态 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBEquipmentNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBEquipmentNetworkComponent();

    /** @param Equipment	当前装备 空引用表示空手 @param Data	具体装备状态 @return 无 */
    void PublishState(ABBBEquipment *Equipment, const TArray<uint8> &Data);

    /** @param OutLifetimeProps	复制属性列表 @return 无 */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
    /** @param State	本机操控端产生的完整状态 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitState(FBBBEquipmentNetworkState State);

    /** 只复制当前结果 不保留事件历史 */
    UPROPERTY(Replicated)
    FBBBEquipmentNetworkState ReplicatedState;

    /** 发送侧最近一次持有实例 */
    TWeakObjectPtr<ABBBEquipment> PublishedEquipment;

    /** 接收侧发起切换时的旧实例 */
    TWeakObjectPtr<ABBBEquipment> PreviousEquipment;

    /** 已投递持有关系切换的代次 */
    uint64 RequestedGeneration = 0;

    /** 已消费的当前代次版本 */
    uint64 AppliedRevision = 0;
};
