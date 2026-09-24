#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipment.generated.h"

class UBBBEquipmentAnimInstance;
class UBBBEquipmentDefinition;
class UArrowComponent;
class USkeletalMeshComponent;

/** 单件装备的共享演员 配置和固定行为入口 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API ABBBEquipment : public AActor
{
    GENERATED_BODY()

public:
    ABBBEquipment();

    /** @return 装备配置标识 */
    FName GetEquipmentId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const
    {
        return Definition;
    }

    /** @return 持有角色当前是否只执行镜像还原 */
    bool IsMirror() const;

    /** @return 当前是否是持有者的激活装备 */
    bool IsEquipped() const;

    /** @return 装备骨骼网格 */
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /** @return 装备动画实例 */
    UBBBEquipmentAnimInstance *GetEquipmentAnimationInstance() const;

    /** 提交持有关系建立后的装备表现 @return 无 */
    virtual void SubmitEquipInput();

    /** 提交本机主行为 @return 无 */
    virtual void SubmitPrimaryInput();

    /** 提交本机换弹行为 @return 无 */
    virtual void SubmitReloadInput();

    /** 卸下前收束自身输入和表现 @return 无 */
    virtual void OnUnequipped();

    /** @param Data	收到的当前装备状态 @return 是否接受 */
    virtual bool SubmitNetworkPayload(const TArray<uint8> &Data);

private:
    friend class FBBBCharacterEquipmentLifecycleProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;

    /** @return 角色应链接的动画层类型 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;

    /** 初始化公共组件与具体装备状态 @return 是否初始化成功 */
    bool InitializeEquipment();

protected:
    /** 初始化具体装备状态 @return 是否初始化成功 */
    virtual bool InitializeRuntimeData();

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBBBEquipmentAnimInstance> EquipmentAnimationInstance = nullptr;
};
