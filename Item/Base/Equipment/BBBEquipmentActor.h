
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/Base/Item/BBBItemInstance.h"
#include "BBBWork/UBBBNexus/Item/Base/Item/BBBItemTypes.h"
#include "BBBEquipmentActor.generated.h"
struct FBBBCharacterAnimationRequest;
class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;

UCLASS(Abstract)
class ABBB_EVAC_API ABBBEquipmentActor : public AActor
{
    GENERATED_BODY()
public:

    ABBBEquipmentActor();

    /**
     * 初始化本地完整装备实例
     * @param InItemInstance	物品实例
     * @param InCharacterAPI	角色能力接口
     */
    virtual void InitializeRuntimeEquipment(const FBBBItemInstance &InItemInstance, FBBBCharacterExternalAPI &InCharacterAPI);

    /**
     * 初始化远端表现镜像
     * @param InDefinition	装备静态定义
     * @param InCharacterAPI	角色能力接口
     */
    virtual void InitializeEquipmentMirror(const UBBBEquipmentDefinition &InDefinition, FBBBCharacterExternalAPI &InCharacterAPI);

    virtual void Tick(float DeltaSeconds) override;

    virtual void Equip();

    /** @return 是否成功执行开火 */
    virtual bool Fire();

    /** @return 是否成功执行换弹 */
    virtual bool Reload();

    /** 播放远端开火表现 */
    virtual void PresentFire();

    /** 播放远端换弹表现 */
    virtual void PresentReload();

    /** @return 当前装备使用的静态定义 */
    const UBBBEquipmentDefinition *GetEquipmentDefinition() const;

    /** @return 当前装备实体的数据模式 */
    EBBBEquipmentInstanceMode GetInstanceMode() const;
protected:

    void InitializeCommon(
        const UBBBEquipmentDefinition &InDefinition,
        FBBBCharacterExternalAPI &InCharacterAPI,
        EBBBEquipmentInstanceMode InInstanceMode);

    void QueueMontage(const FBBBCharacterAnimationRequest &Request) const;

    UPROPERTY()
    FBBBItemInstance ItemInstance;

    const UBBBEquipmentDefinition *EquipmentDefinition = nullptr;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;

    EBBBEquipmentInstanceMode InstanceMode = EBBBEquipmentInstanceMode::Runtime;
};
