#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentActor.generated.h"

class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;
class UBBBEquipmentInstance;
struct FBBBCharacterAnimationRequest;

UCLASS(Abstract)
class ABBB_EVAC_API ABBBEquipmentActor : public AActor
{
    GENERATED_BODY()

public:

    /**
     * 构造装备模型 默认关闭 Tick 且不参与网络复制
     */
    ABBBEquipmentActor();

    /**
     * 初始化装备模型依赖
     * @param InEquipmentInstance	所属装备实例
     * @param InCharacterAPI	角色能力接口
     */
    virtual void Initialize(UBBBEquipmentInstance &InEquipmentInstance, FBBBCharacterExternalAPI &InCharacterAPI);

    /**
     * 每帧更新装备模型
     * @param DeltaSeconds	帧间隔时间
     */
    virtual void Tick(float DeltaSeconds) override;

    /**
     * 播放装备过渡 Montage
     */
    virtual void Equip();

    /**
     * 执行开火 基类默认返回 false
     * @return 是否成功开火
     */
    virtual bool Fire();

    /**
     * 执行装填 基类默认返回 false
     * @return 是否成功开始装填
     */
    virtual bool Reload();

    /**
     * 播放开火表现 基类默认空实现
     */
    virtual void PresentFire();

    /**
     * 播放装填表现 基类默认空实现
     */
    virtual void PresentReload();

    /**
     * 获取装备静态配置
     * @return 装备静态配置
     */
    const UBBBEquipmentDefinition *GetEquipmentDefinition() const;

protected:

    /**
     * 向角色动画系统提交 Montage 请求
     * @param Request	角色动画请求
     */
    void QueueMontage(const FBBBCharacterAnimationRequest &Request) const;

    UPROPERTY()
    TObjectPtr<UBBBEquipmentInstance> EquipmentInstance = nullptr;

    const UBBBEquipmentDefinition *EquipmentDefinition = nullptr;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;
};
