#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/RemoteMessage/Use/FBBBConsumableStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/AuthorityFact/Use/FBBBConsumableStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableShowLocalControlPacket.h"
#include "BBBConsumable.generated.h"
class UBBBConsumableDefinition;
class UStaticMeshComponent;
class UAnimInstance;
class FBBBConsumableInitializer;
class FBBBConsumableUpdatePipeline;
class FBBBConsumableShutdown;
class UBBBConsumableNetworkComponent;
/** 不具有物品动画实例的消耗品公共根 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ABBB_EVAC_API ABBBConsumable : public AActor
{
    GENERATED_BODY()
public:
    ABBBConsumable();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** @return 本类网络协议组件 */
    UBBBConsumableNetworkComponent *GetNetworkComponent() const;
    /** @return 外侧消耗品定义 */
    UBBBConsumableDefinition *GetDefinition() const;
    /** @return 已成立的初始化结果 */
    bool IsInitialized() const;
    /** @return 手持模型组件 */
    UStaticMeshComponent *GetMesh() const;
    /** @return 角色使用的动画层 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;
    /** @param OutOffset	局部挂接偏移 @return 配置是否有效 */
    bool TryGetAttachmentOffset(FTransform &OutOffset) const;
    /** @return 手持网格隐藏事实 */
    virtual bool IsMeshHidden() const PURE_VIRTUAL(ABBBConsumable::IsMeshHidden, return false;);
    /** @return 当前动作是否阻止主动切换 */
    virtual bool IsUsing() const PURE_VIRTUAL(ABBBConsumable::IsUsing, return false;);
    /** @return 当前手持模型是否已释放 */
    virtual bool IsReleased() const PURE_VIRTUAL(ABBBConsumable::IsReleased, return false;);
    /** @return 已开始的使用序号 */
    virtual int32 GetUseSerial() const PURE_VIRTUAL(ABBBConsumable::GetUseSerial, return 0;);

    /** @param Packet	外部输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }

protected:
    /** 由具体实例构造的协议组件 */
    UPROPERTY(VisibleAnywhere, Category="BBB|消耗品", meta=(DisplayName="消耗品协议组件"))
    TObjectPtr<UBBBConsumableNetworkComponent> NetworkComponent = nullptr;
    virtual bool InitializeRuntimeData() PURE_VIRTUAL(ABBBConsumable::InitializeRuntimeData, return false;);
    virtual void UpdateRuntimeData() PURE_VIRTUAL(ABBBConsumable::UpdateRuntimeData, );
    virtual void ShutdownRuntimeData() PURE_VIRTUAL(ABBBConsumable::ShutdownRuntimeData, );
    virtual void QueueInput(FBBBConsumableHideLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumableShowLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumableEquipLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumableReleaseLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumableStateRemoteMessagePacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
    virtual void QueueInput(FBBBConsumableStateAuthorityFactPacket<> Packet) PURE_VIRTUAL(ABBBConsumable::QueueInput, );
private:
    friend class FBBBConsumableInitializer;
    friend class FBBBConsumableUpdatePipeline;
    friend class FBBBConsumableShutdown;
    /** 共享静态配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (AllowPrivateAccess = "true", DisplayName = "定义资产"))
    TObjectPtr<UBBBConsumableDefinition> Definition = nullptr;
    /** 消耗品显示组件 不创建动画实例 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|消耗品", meta = (AllowPrivateAccess = "true", DisplayName = "手持网格"))
    TObjectPtr<UStaticMeshComponent> ItemMesh = nullptr;
    /** 初始化结果 */
    bool bInitialized = false;
};
