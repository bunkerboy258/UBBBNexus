
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"
#include "BBBWeaponActor.generated.h"
class ABBBBulletActor;
class UArrowComponent;
class UBBBFireFragment;
class UBBBFireRuntimeData;
class UBBBMagazineFragment;
class UBBBMagazineRuntimeData;
class UBBBSingleProjectileFireFragment;
class USoundBase;
class UStaticMeshComponent;
struct FBBBShotEvent;
struct FBBBCharacterAnimationRequest;

UCLASS()
class ABBB_EVAC_API ABBBWeaponActor : public ABBBEquipmentActor
{
    GENERATED_BODY()
public:

    /**
     * 构造武器模型 创建根组件与武器网格
     */
    ABBBWeaponActor();

    /**
     * 初始化武器模型并绑定 Fragment 与运行数据
     * @param InEquipmentInstance	所属装备实例
     * @param InCharacterAPI	角色能力接口
     */
    virtual void Initialize(UBBBEquipmentInstance &InEquipmentInstance, FBBBCharacterExternalAPI &InCharacterAPI) override;

    /**
     * 每帧驱动弹匣逻辑更新
     * @param DeltaSeconds	帧间隔时间
     */
    virtual void Tick(float DeltaSeconds) override;

    /**
     * 校验弹药后执行开火并消耗弹匣弹药
     * @return 是否成功开火
     */
    virtual bool Fire() override;

    /**
     * 请求弹匣装填
     * @return 是否成功开始装填
     */
    virtual bool Reload() override;

    /**
     * 播放开火表现
     */
    virtual void PresentFire() override;

    /**
     * 播放装填表现
     */
    virtual void PresentReload() override;

    /**
     * 获取武器静态网格组件
     * @return 武器静态网格组件
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Weapon")
    UStaticMeshComponent *GetWeaponMesh() const
    {

        return WeaponMesh;
    }

    /**
     * 获取武器网格插槽的世界变换 插槽不存在时回退到 Actor 变换
     * @param SocketName	插槽名称
     * @return 插槽世界变换
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Weapon")
    FTransform GetSocketTransform(FName SocketName) const;

    /**
     * 获取枪口插槽的世界变换
     * @param MuzzleSocketName	枪口插槽名称
     * @return 枪口插槽世界变换
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Weapon")
    FTransform GetMuzzleTransform(FName MuzzleSocketName) const;
protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Weapon")
    TObjectPtr<UArrowComponent> WeaponRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Weapon")
    TObjectPtr<UStaticMeshComponent> WeaponMesh;
private:

    friend class UBBBSingleProjectileFireFragment;

    friend class UBBBMagazineFragment;

    /**
     * 生成 Bullet 并按射击事件初始化
     * @param BulletActorClass	Bullet 类型
     * @param MuzzleSpeed	枪口初速
     * @param ShotEvent	射击事件
     * @return 是否成功生成 Bullet
     */
    bool SpawnBullet(TSubclassOf<ABBBBulletActor> BulletActorClass, float MuzzleSpeed, const FBBBShotEvent &ShotEvent);

    /**
     * 向角色动画系统提交 Montage 请求
     * @param Request	角色动画请求
     */
    void SubmitMontage(const FBBBCharacterAnimationRequest &Request) const;

    /**
     * 在枪口位置播放开火音效
     * @param Sound	开火音效
     * @param MuzzleSocketName	枪口插槽名称
     */
    void PlayFireSound(USoundBase *Sound, FName MuzzleSocketName) const;

    /**
     * 向角色提交开火后坐力
     * @param Impulse	后坐力冲量 垂直与水平分量
     * @param RecoverySpeed	后坐力恢复速度
     */
    void SubmitRecoil(const FVector2D &Impulse, float RecoverySpeed) const;

    /**
     * 提交物品 IK 屏蔽请求
     * @param bBlockItemIK	是否屏蔽物品 IK
     */
    void SubmitItemIKBlock(bool bBlockItemIK) const;

    /**
     * 从装备静态配置绑定 Fire 与 Magazine Fragment
     */
    void BindDefinitionFragments();

    /**
     * 从装备实例绑定 Fire 与 Magazine 运行数据
     */
    void BindRuntimeData();

    const UBBBFireFragment *FireFragment = nullptr;

    const UBBBMagazineFragment *MagazineFragment = nullptr;

    UBBBFireRuntimeData *FireRuntimeData = nullptr;

    UBBBMagazineRuntimeData *MagazineRuntimeData = nullptr;
};
