
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponActor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBFireFragment.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Magazine/BBBMagazineFragment.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Magazine/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBFireTypes.h"
#include "BBBWork/UBBBNexus/Item/RuntimeData/BBBItemRuntimeData.h"
#include "Kismet/GameplayStatics.h"

ABBBWeaponActor::ABBBWeaponActor()
{

    WeaponRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("WeaponRoot"));
    SetRootComponent(WeaponRoot);

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(WeaponRoot);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponMesh->SetGenerateOverlapEvents(false);
}

void ABBBWeaponActor::InitializeEquipment(
    const FBBBItemInstance &InItemInstance,
    FBBBCharacterExternalAPI &InCharacterAPI)
{

    Super::InitializeEquipment(InItemInstance, InCharacterAPI);

    if (!ensureMsgf(ItemInstance.Definition, TEXT("[UBBBI]Weapon item instance has no definition")))
    {
        SetActorTickEnabled(false);
        return;
    }

    if (!ensureMsgf(ItemInstance.RuntimeData, TEXT("[UBBBI]Weapon item instance has no runtime data")))
    {
        SetActorTickEnabled(false);
        return;
    }

    FireFragment = ItemInstance.Definition->FindFragment<UBBBFireFragment>();

    MagazineFragment = ItemInstance.Definition->FindFragment<UBBBMagazineFragment>();

    FireRuntimeData = ItemInstance.RuntimeData->FindRuntimeData<UBBBFireRuntimeData>();

    MagazineRuntimeData = ItemInstance.RuntimeData->FindRuntimeData<UBBBMagazineRuntimeData>();

    if (!ensureMsgf(
            FireFragment && MagazineFragment && FireRuntimeData && MagazineRuntimeData,
            TEXT("[UBBBI]Weapon definition missing required fire or magazine fragments/runtime data")))
    {
        SetActorTickEnabled(false);
        return;
    }
}

void ABBBWeaponActor::Tick(float DeltaSeconds)
{

    Super::Tick(DeltaSeconds);

    if (!ensureMsgf(MagazineFragment && MagazineRuntimeData, TEXT("[UBBBI]Weapon tick skipped because magazine fragment or runtime data is missing")))
    {
        return;
    }

    MagazineFragment->Update(*this, *MagazineRuntimeData);
}

bool ABBBWeaponActor::Fire()
{

    if (!ensureMsgf(
            FireFragment && MagazineFragment && FireRuntimeData && MagazineRuntimeData,
            TEXT("[UBBBI]Weapon fire failed: missing fire or magazine fragments/runtime data")))
    {
        return false;
    }

    if (!MagazineFragment->CanConsumeRound(*MagazineRuntimeData))
    {
        return false;
    }

    FBBBShotEvent ShotEvent;

    if (!FireFragment->Fire(*this, *FireRuntimeData, ShotEvent))
    {
        return false;
    }

    MagazineFragment->ConsumeRound(*MagazineRuntimeData);

    PublishFireEvent();
    return true;
}

bool ABBBWeaponActor::Reload()
{

    if (!ensureMsgf(MagazineFragment && MagazineRuntimeData, TEXT("[UBBBI]Weapon reload failed: missing magazine fragment or runtime data")))
    {
        return false;
    }

    return MagazineFragment->Reload(*this, *MagazineRuntimeData);
}

void ABBBWeaponActor::PresentFire()
{

    if (!ensureMsgf(FireFragment, TEXT("[UBBBI]Weapon fire presentation failed because fire fragment is missing")))
    {
        return;
    }

    FireFragment->Present(*this);
}

void ABBBWeaponActor::PresentReload()
{

    if (!ensureMsgf(MagazineFragment && MagazineRuntimeData, TEXT("[UBBBI]Weapon reload presentation failed because magazine fragment or runtime data is missing")))
    {
        return;
    }

    MagazineFragment->PresentReload(*this, *MagazineRuntimeData);
}

bool ABBBWeaponActor::SpawnBullet(
    TSubclassOf<ABBBBulletActor> BulletActorClass,
    float MuzzleSpeed,
    const FBBBShotEvent &ShotEvent)
{

    if (!BulletActorClass)
    {
        return false;
    }

    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || !GetWorld())
    {
        return false;
    }

    FActorSpawnParameters Parameters;
    Parameters.Owner = Character;
    Parameters.Instigator = Character;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBBulletActor *Bullet = GetWorld()->SpawnActor<ABBBBulletActor>(
        BulletActorClass,
        FTransform(ShotEvent.ShotDirection.Rotation(), ShotEvent.MuzzleLocation),
        Parameters);

    if (!Bullet)
    {
        return false;
    }

    Bullet->InitializeBullet(
        FVector(ShotEvent.ShotDirection) * MuzzleSpeed,
        Character,
        this);
    return true;
}

void ABBBWeaponActor::SubmitMontage(const FBBBCharacterAnimationRequest &Request) const
{

    QueueMontage(Request);
}

void ABBBWeaponActor::PlayFireSound(USoundBase *Sound, FName MuzzleSocketName) const
{

    if (!Sound || !GetWorld())
    {
        return;
    }

    FVector SoundLocation = GetActorLocation();

    if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
    {
        SoundLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
    }

    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, SoundLocation);
}

void ABBBWeaponActor::SubmitRecoil(const FVector2D &Impulse, float RecoverySpeed) const
{

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Weapon recoil skipped because character API is null")))
    {
        return;
    }

    CharacterAPI->SubmitCameraRecoil(Impulse, RecoverySpeed);
}

void ABBBWeaponActor::PublishFireEvent() const
{

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Weapon fire event skipped because character API is null")))
    {
        return;
    }

    CharacterAPI->PublishItemFireEvent();
}

void ABBBWeaponActor::PublishReloadEvent() const
{

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Weapon reload event skipped because character API is null")))
    {
        return;
    }

    CharacterAPI->PublishReloadEvent();
}

void ABBBWeaponActor::SubmitItemIKBlock(bool bBlockItemIK) const
{

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Weapon IK skipped because character API is null")))
    {
        return;
    }

    CharacterAPI->SubmitItemIKBlockRequest(bBlockItemIK);
}

FTransform ABBBWeaponActor::GetSocketTransform(FName SocketName) const
{

    if (WeaponMesh && WeaponMesh->DoesSocketExist(SocketName))
    {
        return WeaponMesh->GetSocketTransform(SocketName, RTS_World);
    }

    return GetActorTransform();
}

FTransform ABBBWeaponActor::GetMuzzleTransform(FName MuzzleSocketName) const
{

    return GetSocketTransform(MuzzleSocketName);
}
