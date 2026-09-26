#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipment.generated.h"

class UBBBEquipmentAnimInstance;
class UBBBEquipmentDefinition;
class UArrowComponent;
class USkeletalMeshComponent;
class FBBBEquipmentInitializer;
struct FBBBEquipmentEquipLocalControlPacket;
struct FBBBEquipmentEquipAuthorityFactPacket;
struct FBBBEquipmentPrimaryLocalControlPacket;
struct FBBBEquipmentReloadLocalControlPacket;
struct FBBBEquipmentStateAuthorityFactPacket;

/** 单件装备的共享演员 配置和固定行为入口 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API ABBBEquipment : public AActor
{
    GENERATED_BODY()

public:
    ABBBEquipment();

    /** 装备开始运行时初始化自身 @return 无 */
    virtual void BeginPlay() override;

    /** @return 装备配置标识 */
    FName GetEquipmentId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const
    {
        return Definition;
    }

    /** @return 装备自身是否完成初始化 */
    bool IsInitialized() const
    {
        return bInitialized;
    }

    /**
     * 读取装备挂接偏移
     * @param OutOffset	有效时写入挂接偏移
     * @return 是否具有有效配置
     */
    bool TryGetAttachmentOffset(FTransform &OutOffset) const;

    /** @return 角色应链接的动画层类型 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;

    /** @return 持有角色当前是否只执行镜像还原 */
    bool IsMirror() const;

    /** @return 当前是否是持有者的激活装备 */
    bool IsEquipped() const;

    /** @return 装备骨骼网格 */
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /** @return 装备动画实例 */
    UBBBEquipmentAnimInstance *GetEquipmentAnimationInstance() const;

    /**
     * 提交装备输入或网络传输结果
     * @param Packet	待提交的数据
     * @return 是否接受
     */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        if (!ensureMsgf(IsInGameThread() && Packet.IsValid(), TEXT("装备输入线程或数据无效")))
        {
            return false;
        }

        return QueueInput(Forward<TPacket>(Packet));
    }

    /** 卸下前收束自身输入和表现 @return 无 */
    virtual void OnUnequipped();

private:
    friend class FBBBEquipmentInitializer;

protected:
    /** 初始化具体装备状态 @return 是否初始化成功 */
    virtual bool InitializeRuntimeData();

    /** @param Packet	装备表现请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentEquipLocalControlPacket Packet);

    /** @param Packet	权威装备表现请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentEquipAuthorityFactPacket Packet);

    /** @param Packet	主行为请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentPrimaryLocalControlPacket Packet);

    /** @param Packet	换弹请求 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentReloadLocalControlPacket Packet);

    /** @param Payload	当前网络结果 @return 是否接受 */
    virtual bool QueueInput(FBBBEquipmentStateAuthorityFactPacket Payload);

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBBBEquipmentAnimInstance> EquipmentAnimationInstance = nullptr;

    /** 装备初始化结果 */
    UPROPERTY(Transient)
    bool bInitialized = false;
};
