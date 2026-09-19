#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Fragment/BBBSingleProjectileFireFragment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireContext.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

FBBBSingleProjectileFireFragment::FBBBSingleProjectileFireFragment()
{
    // 使用默认的子弹演员作为投射物实现
    BulletActorClass = ABBBBulletActor::StaticClass();
}

bool FBBBSingleProjectileFireFragment::CanFire(
    const int32 LoadedAmmo,
    const float LastFireTimeSeconds,
    const float WorldTime) const
{
    // 弹药充足且达到开火间隔时才允许开火
    return LoadedAmmo > 0
        && WorldTime - LastFireTimeSeconds >= FMath::Max(FireInterval, 0.01f);
}

bool FBBBSingleProjectileFireFragment::Fire(FBBBEquipmentFireContext &Context) const
{
    // 开火前确认枪口插槽可用
    if (!ensureMsgf(!MuzzleSocketName.IsNone() && Context.WeaponMesh.DoesSocketExist(MuzzleSocketName),
        TEXT("[UBBBE]Equipment muzzle socket '%s' is missing"), *MuzzleSocketName.ToString()))
    {
        return false;
    }

    const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(MuzzleSocketName, RTS_World);
    if (!Context.bIsMirror)
    {
        // 本地实例负责生成投射物并设置归属
        if (!ensureMsgf(BulletActorClass, TEXT("[UBBBE]Projectile fire requires a bullet class")))
        {
            return false;
        }

        APawn *OwnerPawn = Cast<APawn>(Context.Instance.GetOwner());
        if (!ensureMsgf(OwnerPawn, TEXT("[UBBBE]Projectile fire requires a pawn holder")))
        {
            return false;
        }

        FActorSpawnParameters Parameters;
        Parameters.Owner = OwnerPawn;
        Parameters.Instigator = OwnerPawn;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABBBBulletActor *Bullet = Context.World.SpawnActor<ABBBBulletActor>(
            BulletActorClass, MuzzleTransform, Parameters);
        if (!ensureMsgf(Bullet, TEXT("[UBBBE]Equipment projectile spawn failed")))
        {
            return false;
        }

        Bullet->InitializeBullet(
            MuzzleTransform.GetUnitAxis(EAxis::X) * MuzzleSpeed,
            OwnerPawn,
            &Context.Instance);
    }

    // 本地和镜像实例都播放开火表现
    if (FireSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(&Context.World, FireSound, MuzzleTransform.GetLocation());
    }

    if (FireMontage)
    {
        BBBCharacterMontageInput::Submit(
            Context.CharacterAPI,
            *FireMontage,
            1.0f,
            Context.Sequence,
            false);
    }

    // 本地实例负责消耗弹药并提交后坐力
    if (!Context.bIsMirror)
    {
        const FVector2D RecoilImpulse(
            VerticalRecoilAmount + FMath::FRandRange(-VerticalRecoilRandom, VerticalRecoilRandom),
            HorizontalRecoilAmount + FMath::FRandRange(-HorizontalRecoilRandom, HorizontalRecoilRandom));
        Context.LoadedAmmo--;
        FBBBCameraPacket CameraPacket;
        CameraPacket.Impulse = RecoilImpulse;
        CameraPacket.RecoverySpeed = RecoilRecoverySpeed;
        Context.CharacterAPI.SubmitInput(CameraPacket);
    }

    // 记录本次开火时间供下一次开火判断
    Context.LastFireTimeSeconds = Context.World.GetTimeSeconds();
    return true;
}
