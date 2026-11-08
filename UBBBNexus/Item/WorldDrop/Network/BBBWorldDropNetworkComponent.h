#pragma once
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Definitions/BBBWorldDropSnapshot.h"
#include "BBBWorldDropNetworkComponent.generated.h"
/** 世界掉落物完整当前结果的 UE 复制边界 */
UCLASS()
class ABBB_EVAC_API UBBBWorldDropNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    UBBBWorldDropNetworkComponent();
    /** @param Snapshot\t已经成立的完整结果 @return 无 */
    void Publish(const FBBBWorldDropSnapshot &Snapshot);
    //~ Begin UActorComponent Interface
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &Out) const override;
    //~ End UActorComponent Interface
private:
    /** @return 无 只贡献当前结果还原输入 */
    UFUNCTION()
    void OnRep_Snapshot();
    /** 复制传输用的完整结果值副本 */
    UPROPERTY(ReplicatedUsing = OnRep_Snapshot)
    FBBBWorldDropSnapshot ReplicatedSnapshot;
};
