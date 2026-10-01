#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterOldMagazineDisplayAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterMagazineMotionComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"

namespace
{
    const FName OldMagazineTag(TEXT("BBB_CharacterOldHandMagazine"));

    UBBBCharacterMagazineMotionComponent *FindOldMagazine(USkeletalMeshComponent &HandMesh)
    {
        TArray<USceneComponent *> Children;
        HandMesh.GetChildrenComponents(false, Children);

        for (USceneComponent *Child : Children)
        {
            UBBBCharacterMagazineMotionComponent *Magazine = Cast<UBBBCharacterMagazineMotionComponent>(Child);
            if (Magazine && Magazine->ComponentHasTag(OldMagazineTag))
            {
                return Magazine;
            }
        }

        return nullptr;
    }
}

void UBBBCharacterOldMagazineDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBCharacter *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    if (!Character)
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MagazineMesh
        && MeshComp->GetBoneIndex(HandBoneName) != INDEX_NONE
        && !HandMagazineTransform.ContainsNaN()
        && FMath::IsFinite(DroppedLifeSeconds) && DroppedLifeSeconds > 0.0f
        && !FindOldMagazine(*MeshComp),
        TEXT("旧弹匣角色表现缺少网格 骨骼 位置或存在重复组件 %s"), *Character->GetName()))
    {
        return;
    }

    // 旧弹匣不是从武器对象转移而来 角色动画仅生成独立的手持视觉对象
    FName MagazineName = NAME_None;
    EObjectFlags MagazineFlags = RF_Transient;

#if WITH_EDITOR
    // 编辑器中的临时实例使用可辨认的名称和事务标记 便于选中后调整手部相对变换
    MagazineName = MakeUniqueObjectName(Character, UBBBCharacterMagazineMotionComponent::StaticClass(), TEXT("BBB_OldHandMagazine"));
    MagazineFlags |= RF_Transactional;
#endif

    UBBBCharacterMagazineMotionComponent *Magazine = NewObject<UBBBCharacterMagazineMotionComponent>(Character, MagazineName, MagazineFlags);
    if (!ensureMsgf(Magazine, TEXT("旧弹匣角色表现创建组件失败 %s"), *Character->GetName()))
    {
        return;
    }

    Magazine->ComponentTags.Add(OldMagazineTag);
    Magazine->SetStaticMesh(MagazineMesh);
    Magazine->SetMobility(EComponentMobility::Movable);
    Magazine->SetCollisionEnabled(ECollisionEnabled::NoCollision);

#if WITH_EDITOR
    // 实例组件注册使详情面板开放变换编辑 销毁时引擎同步移除实例列表 不写回通知资产
    Character->AddInstanceComponent(Magazine);
    Magazine->bSelectable = true;
    Magazine->bWantsEditorEffects = true;
#endif

    Magazine->RegisterComponent();
    if (!ensureMsgf(Magazine->IsRegistered()
        && Magazine->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBoneName),
        TEXT("旧弹匣角色表现注册或手部附着失败 %s"), *Character->GetName()))
    {
        Magazine->DestroyComponent();
        return;
    }

    Magazine->SetRelativeTransform(HandMagazineTransform);
    Magazine->AddTickPrerequisiteComponent(MeshComp);
    Magazine->SampleMotion();

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine spawned on character hand Character=%s"), *Character->GetName());
}

void UBBBCharacterOldMagazineDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *Animation,
    const FAnimNotifyEventReference &EventReference)
{
    if (!MeshComp)
    {
        return;
    }

    UBBBCharacterMagazineMotionComponent *Magazine = FindOldMagazine(*MeshComp);
    ABBBCharacter *Character = Cast<ABBBCharacter>(MeshComp->GetOwner());
    if (!Magazine || !Character)
    {
        return;
    }

    // 蒙太奇被打断时仍会结束通知区间 只有经过甩出帧才产生世界掉落物
    UAnimMontage *Montage = Cast<UAnimMontage>(Animation);
    UAnimInstance *AnimInstance = MeshComp->GetAnimInstance();
    const FAnimNotifyEvent *Event = EventReference.GetNotify();
    const bool bReachedRelease = Montage && AnimInstance && Event
        && AnimInstance->Montage_GetPosition(Montage) + 0.002f >= Event->GetEndTriggerTime();

    if (!bReachedRelease)
    {
        Magazine->DestroyComponent();
        UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine display cancelled Character=%s"), *Character->GetName());
        return;
    }

    UWorld *World = Character->GetWorld();
    if (!ensureMsgf(World && MagazineMesh, TEXT("旧弹匣角色表现无法创建掉落物 %s"), *Character->GetName()))
    {
        Magazine->DestroyComponent();
        return;
    }

    // 通知结束派发时姿势已跨过释放点 只刷新掉落位置 保留释放前的甩动速度
    Magazine->UpdateComponentToWorld();
    const FTransform DropWorld = Magazine->GetComponentTransform();
    AStaticMeshActor *Dropped = World->SpawnActor<AStaticMeshActor>(DropWorld.GetLocation(), DropWorld.Rotator());
    if (ensureMsgf(Dropped, TEXT("旧弹匣角色表现生成掉落对象失败 %s"), *Character->GetName()))
    {
        UStaticMeshComponent *DroppedMesh = Dropped->GetStaticMeshComponent();
        DroppedMesh->SetMobility(EComponentMobility::Movable);
        DroppedMesh->SetStaticMesh(MagazineMesh);
        DroppedMesh->SetWorldTransform(DropWorld);
        DroppedMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
        DroppedMesh->SetSimulatePhysics(true);
        DroppedMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
        DroppedMesh->SetPhysicsLinearVelocity(Magazine->GetReleaseLinearVelocity());
        DroppedMesh->SetPhysicsAngularVelocityInRadians(Magazine->GetReleaseAngularVelocity());
        Dropped->SetLifeSpan(DroppedLifeSeconds);
        UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Release motion Character=%s MontageTime=%.6f EndTime=%.6f SampleAge=%.6f Linear=%s Angular=%s"),
            *Character->GetName(), AnimInstance->Montage_GetPosition(Montage), Event->GetEndTriggerTime(),
            World->GetTimeSeconds() - Magazine->GetMotionSampleTime(),
            *Magazine->GetReleaseLinearVelocity().ToString(), *Magazine->GetReleaseAngularVelocity().ToString());
    }

    Magazine->DestroyComponent();
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Old magazine dropped Character=%s"), *Character->GetName());
}
