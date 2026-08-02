
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
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

UCLASS(Abstract)
class ABBB_EVAC_API ABBBWeaponActor : public ABBBEquipmentActor
{
    GENERATED_BODY()
public:

    ABBBWeaponActor();

    virtual void InitializeEquipment(const FBBBItemInstance &InItemInstance, FBBBCharacterExternalAPI &InCharacterAPI) override;

    virtual void Tick(float DeltaSeconds) override;

    virtual bool Fire();

    virtual bool Reload();

    virtual void PresentFire();

    virtual void PresentReload();

    UFUNCTION(BlueprintCallable, Category = "BBB|Weapon")
    UStaticMeshComponent *GetWeaponMesh() const
    {

        return WeaponMesh;
    }

    UFUNCTION(BlueprintCallable, Category = "BBB|Weapon")
    FTransform GetSocketTransform(FName SocketName) const;

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

    bool SpawnBullet(TSubclassOf<ABBBBulletActor> BulletActorClass, float MuzzleSpeed, const FBBBShotEvent &ShotEvent);

    void SubmitMontage(const FBBBCharacterAnimationRequest &Request) const;

    void PlayFireSound(USoundBase *Sound, FName MuzzleSocketName) const;

    void SubmitRecoil(const FVector2D &Impulse, float RecoverySpeed) const;

    void PublishFireEvent() const;

    void PublishReloadEvent() const;

    void SubmitItemIKBlock(bool bBlockItemIK) const;

    const UBBBFireFragment *FireFragment = nullptr;

    const UBBBMagazineFragment *MagazineFragment = nullptr;

    UBBBFireRuntimeData *FireRuntimeData = nullptr;

    UBBBMagazineRuntimeData *MagazineRuntimeData = nullptr;
};
