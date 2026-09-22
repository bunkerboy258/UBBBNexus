#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterBasicActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "BBBWork/UBBBNexus/MonsterMass/Input/MonsterDamageInput.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

AMonsterBasicActor::AMonsterBasicActor()
{
    // 表现 Actor 的位置和生命周期由 Mass Representation 管理
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);

    // 根碰撞体只在表现层需要时开启查询碰撞
    MonsterCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MonsterCollision"));
    MonsterCollision->InitSphereRadius(45.0f);
    MonsterCollision->SetCollisionProfileName(TEXT("Pawn"));
    MonsterCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    MonsterCollision->SetGenerateOverlapEvents(true);
    SetRootComponent(MonsterCollision);

    // 网格关闭自身碰撞，避免与根碰撞体重复计算
    MonsterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MonsterMesh"));
    MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterMesh->SetGenerateOverlapEvents(false);
    MonsterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MonsterMesh->SetupAttachment(MonsterCollision);

    // 状态组件负责把 Mass 状态转换为动画
    MonsterPresentation = CreateDefaultSubobject<UMonsterPresentationComponent>(TEXT("MonsterPresentation"));
}

float AMonsterBasicActor::TakeDamage(
    const float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    if (!FMath::IsFinite(DamageAmount) || DamageAmount <= 0.0f || !CanBeDamaged())
    {
        return 0.0f;
    }

    FMonsterDamageRequest Request;
    Request.Damage = DamageAmount;
    Request.DamageCauser = DamageCauser;
    Request.Instigator = EventInstigator;

    FHitResult Hit;
    DamageEvent.GetBestHitInfo(this, DamageCauser, Hit, Request.HitDirection);
    Request.HitLocation = Hit.ImpactPoint;

    if (!UMonsterDamageInput::SubmitDamage(this, Request))
    {
        return 0.0f;
    }

    // 接收成功后才广播标准伤害通知 监听方不得把通知重复转回伤害入口
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 返回已接受的伤害数值
    return DamageAmount;
}

USkeletalMeshComponent* AMonsterBasicActor::GetMonsterMesh() const
{
    return MonsterMesh;
}

UMonsterPresentationComponent* AMonsterBasicActor::GetMonsterPresentation() const
{
    return MonsterPresentation;
}

void AMonsterBasicActor::SetCollisionRadius(const float Radius)
{
    // 表现同步时动态应用实体的避让半径
    MonsterCollision->SetSphereRadius(FMath::Max(Radius, 1.0f));
    MonsterCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
