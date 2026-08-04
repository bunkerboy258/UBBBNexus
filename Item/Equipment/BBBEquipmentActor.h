#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentActor.generated.h"

class ABBBBulletActor;
class FBBBCharacterExternalAPI;
class UArrowComponent;
class UBBBFireOperation;
class UBBBFireRuntimeData;
class UBBBEquipmentOperation;
class UBBBItemDefinition;
class UBBBItemInstance;
class UBBBMagazineOperation;
class UBBBMagazineRuntimeData;
class UBBBSingleProjectileFireOperation;
class USoundBase;
class UStaticMeshComponent;
struct FBBBCharacterAnimationRequest;

/** 装备实体与操作域调用入口 */
UCLASS()
class ABBB_EVAC_API ABBBEquipmentActor : public AActor
{
    GENERATED_BODY()

public:
    /** 构造装备实体与显示组件 */
    ABBBEquipmentActor();

    /**
     * 初始化物品实例与角色能力
     * @param InItemInstance    所属物品实例
     * @param InCharacterAPI    角色能力接口
     */
    void Initialize(UBBBItemInstance &InItemInstance, FBBBCharacterExternalAPI &InCharacterAPI);

    //~ Begin AActor Interface
    virtual void Tick(float DeltaSeconds) override;
    //~ End AActor Interface

    /** 播放装备过渡 */
    void Equip();

    /** @return 是否成功响应开火操作 */
    bool Fire();

    /** @return 是否成功响应换弹操作 */
    bool Reload();

    /** 响应远端开火表现 */
    void PresentFire();

    /** 响应远端换弹表现 */
    void PresentReload();

    /** @return 物品静态配置 */
    const UBBBItemDefinition *GetItemDefinition() const;

    /** @return 装备显示网格 */
    UStaticMeshComponent *GetEquipmentMesh() const;

    /** @return 是否正在执行装备过渡 */
    bool IsEquipping() const;

private:
    friend class UBBBMagazineOperation;
    friend class UBBBSingleProjectileFireOperation;

    /**
     * 生成投射物
     * @param BulletActorClass  投射物类型
     * @param SpawnTransform    生成变换
     * @param MuzzleSpeed       初始速度
     * @return 是否成功生成投射物
     */
    bool SpawnProjectile(
        TSubclassOf<ABBBBulletActor> BulletActorClass,
        const FTransform &SpawnTransform,
        float MuzzleSpeed);

    /**
     * 向角色动画系统提交操作动画
     * @param Request   动画请求
     */
    void QueueOperationMontage(const FBBBCharacterAnimationRequest &Request) const;

    /**
     * 在装备插槽播放操作音效
     * @param Sound         操作音效
     * @param SocketName    音效位置插槽
     */
    void PlayOperationSound(USoundBase *Sound, FName SocketName) const;

    /**
     * 向角色提交相机后坐力
     * @param Impulse           后坐力冲量
     * @param RecoverySpeed     恢复速度
     */
    void SubmitRecoil(const FVector2D &Impulse, float RecoverySpeed) const;

    /**
     * 向角色提交物品逆向动力学屏蔽状态
     * @param bBlockItemIK  是否屏蔽物品逆向动力学
     */
    void SubmitItemIKBlock(bool bBlockItemIK) const;

    /** 装备组件根 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot;

    /** 装备显示网格 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> EquipmentMesh;

    /** 所属物品实例 */
    UPROPERTY()
    TObjectPtr<UBBBItemInstance> ItemInstance = nullptr;

    /** 物品静态配置 */
    const UBBBItemDefinition *ItemDefinition = nullptr;

    /** 角色能力接口 */
    FBBBCharacterExternalAPI *CharacterAPI = nullptr;

    /** 可装备操作域 */
    const UBBBEquipmentOperation *EquipmentOperation = nullptr;

    /** 开火操作域 */
    const UBBBFireOperation *FireOperation = nullptr;

    /** 弹匣操作域 */
    const UBBBMagazineOperation *MagazineOperation = nullptr;

    /** 开火运行数据 */
    UPROPERTY()
    TObjectPtr<UBBBFireRuntimeData> FireRuntimeData = nullptr;

    /** 弹匣运行数据 */
    UPROPERTY()
    TObjectPtr<UBBBMagazineRuntimeData> MagazineRuntimeData = nullptr;

    /** 是否正在执行装备过渡 */
    bool bIsEquipping = false;

    /** 装备过渡结束时间 */
    float EquipEndTime = 0.0f;
};
