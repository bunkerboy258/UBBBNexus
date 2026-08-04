#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentOperation.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Fire/Base/BBBFireOperation.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Fire/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Magazine/BBBMagazineOperation.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Magazine/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Projectile/BBBBulletActor.h"
#include "Animation/AnimMontage.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ABBBEquipmentActor::ABBBEquipmentActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bReplicates = false;
    SetReplicateMovement(false);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh"));
    EquipmentMesh->SetupAttachment(EquipmentRoot);
    EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentMesh->SetGenerateOverlapEvents(false);
}

void ABBBEquipmentActor::Initialize(
    UBBBItemInstance &InItemInstance,
    FBBBCharacterExternalAPI &InCharacterAPI)
{
    if (!ensureMsgf(InItemInstance.GetDefinition(), TEXT("[UBBBI]Equipment item definition is invalid")))
    {
        return;
    }

    ItemInstance = &InItemInstance;
    ItemDefinition = InItemInstance.GetDefinition();
    CharacterAPI = &InCharacterAPI;
    EquipmentOperation = ItemDefinition->EquipmentOperation;
    FireOperation = ItemDefinition->FireOperation;
    MagazineOperation = ItemDefinition->MagazineOperation;

    UBBBItemRuntimeData *RuntimeData = InItemInstance.GetRuntimeData();
    FireRuntimeData = RuntimeData
        ? RuntimeData->GetFire()
        : nullptr;
    MagazineRuntimeData = RuntimeData
        ? RuntimeData->GetMagazine()
        : nullptr;

    if (RuntimeData
        && !ensureMsgf(
            (!FireOperation || FireRuntimeData) && (!MagazineOperation || MagazineRuntimeData),
            TEXT("[UBBBI]Equipment operation runtime data is incomplete")))
    {
        SetActorTickEnabled(false);
    }
}

void ABBBEquipmentActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsEquipping && GetWorld() && GetWorld()->GetTimeSeconds() >= EquipEndTime)
    {
        bIsEquipping = false;
        EquipEndTime = 0.0f;
    }

    if (MagazineOperation && MagazineRuntimeData)
    {
        MagazineOperation->Update(*this, *MagazineRuntimeData);
    }
}

void ABBBEquipmentActor::Equip()
{
    if (!ensureMsgf(EquipmentOperation, TEXT("[UBBBI]Equipment operation is unavailable")))
    {
        return;
    }

    FBBBCharacterAnimationRequest Request;
    Request.Montage = EquipmentOperation->EquipMontage;
    QueueOperationMontage(Request);

    bIsEquipping = EquipmentOperation->EquipMontage && GetWorld();
    EquipEndTime = bIsEquipping
        ? GetWorld()->GetTimeSeconds() + EquipmentOperation->EquipMontage->GetPlayLength()
        : 0.0f;
}

bool ABBBEquipmentActor::Fire()
{
    if (!ensureMsgf(FireOperation && FireRuntimeData, TEXT("[UBBBI]Equipment fire operation is unavailable")))
    {
        return false;
    }

    if (MagazineOperation
        && MagazineRuntimeData
        && !MagazineOperation->CanConsumeRound(*MagazineRuntimeData))
    {
        return false;
    }

    if (!FireOperation->Fire(*this, *FireRuntimeData))
    {
        return false;
    }

    if (MagazineOperation && MagazineRuntimeData)
    {
        MagazineOperation->ConsumeRound(*MagazineRuntimeData);
    }

    return true;
}

bool ABBBEquipmentActor::Reload()
{
    if (!ensureMsgf(MagazineOperation && MagazineRuntimeData, TEXT("[UBBBI]Equipment magazine operation is unavailable")))
    {
        return false;
    }

    return MagazineOperation->Reload(*this, *MagazineRuntimeData);
}

void ABBBEquipmentActor::PresentFire()
{
    if (FireOperation)
    {
        FireOperation->Present(*this);
    }
}

void ABBBEquipmentActor::PresentReload()
{
    if (MagazineOperation)
    {
        MagazineOperation->PresentReload(*this);
    }
}

const UBBBItemDefinition *ABBBEquipmentActor::GetItemDefinition() const
{
    return ItemDefinition;
}

UStaticMeshComponent *ABBBEquipmentActor::GetEquipmentMesh() const
{
    return EquipmentMesh;
}

bool ABBBEquipmentActor::IsEquipping() const
{
    return bIsEquipping;
}

bool ABBBEquipmentActor::SpawnProjectile(
    TSubclassOf<ABBBBulletActor> BulletActorClass,
    const FTransform &SpawnTransform,
    float MuzzleSpeed)
{
    APawn *OwnerPawn = Cast<APawn>(GetOwner());
    if (!ensureMsgf(BulletActorClass && OwnerPawn && GetWorld(), TEXT("[UBBBI]Projectile spawn dependencies are invalid")))
    {
        return false;
    }

    FActorSpawnParameters Parameters;
    Parameters.Owner = OwnerPawn;
    Parameters.Instigator = OwnerPawn;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBBulletActor *Bullet = GetWorld()->SpawnActor<ABBBBulletActor>(
        BulletActorClass,
        SpawnTransform,
        Parameters);

    if (!ensureMsgf(Bullet, TEXT("[UBBBI]Projectile spawn failed")))
    {
        return false;
    }

    Bullet->InitializeBullet(SpawnTransform.GetUnitAxis(EAxis::X) * MuzzleSpeed, OwnerPawn, this);
    return true;
}

void ABBBEquipmentActor::QueueOperationMontage(const FBBBCharacterAnimationRequest &Request) const
{
    if (!Request.Montage)
    {
        return;
    }

    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Character API is null when queueing equipment montage")))
    {
        return;
    }

    CharacterAPI->QueueMontage(Request);
}

void ABBBEquipmentActor::PlayOperationSound(USoundBase *Sound, FName SocketName) const
{
    if (!Sound || !GetWorld())
    {
        return;
    }

    const FVector SoundLocation = EquipmentMesh && EquipmentMesh->DoesSocketExist(SocketName)
        ? EquipmentMesh->GetSocketLocation(SocketName)
        : GetActorLocation();

    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, SoundLocation);
}

void ABBBEquipmentActor::SubmitRecoil(const FVector2D &Impulse, float RecoverySpeed) const
{
    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Character API is null when submitting recoil")))
    {
        return;
    }

    CharacterAPI->SubmitCameraRecoil(Impulse, RecoverySpeed);
}

void ABBBEquipmentActor::SubmitItemIKBlock(bool bBlockItemIK) const
{
    if (!ensureMsgf(CharacterAPI, TEXT("[UBBBI]Character API is null when submitting item IK state")))
    {
        return;
    }

    CharacterAPI->SubmitItemIKBlockRequest(bBlockItemIK);
}
