#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRiflePresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Animation/BBBRifleAnimInstance.h"

namespace
{
    void ClearMagazineVisual(FBBBRifleUpdateContext &Context, FBBBRifleAnimationState &State)
    {
        if (UStaticMeshComponent *HandMagazine = State.HandMagazine.Get())
        {
            HandMagazine->DestroyComponent();
        }

        State.HandMagazine.Reset();
        if (State.bWasMagazineDetached)
        {
            Context.WeaponMesh.UnHideBoneByName(Context.Definition.MagazineBoneName);
        }
        State.bWasMagazineDetached = false;
        State.bWasMagazineReleased = false;
        State.bWasFreshMagazineHeld = false;
        State.bHasPreviousMagazineHandLocation = false;
    }

    UStaticMeshComponent *CreateHandMagazine(
        FBBBRifleUpdateContext &Context,
        USkeletalMeshComponent &HandMesh,
        const FTransform &WorldTransform)
    {
        UStaticMeshComponent *Magazine = NewObject<UStaticMeshComponent>(&Context.Equipment);
        if (!ensureMsgf(Magazine, TEXT("步枪创建临时弹匣组件失败")))
        {
            return nullptr;
        }

        Magazine->SetStaticMesh(Context.Definition.MagazineMesh);
        Magazine->SetMobility(EComponentMobility::Movable);
        Magazine->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Magazine->RegisterComponent();
        Magazine->SetWorldTransform(WorldTransform);
        Magazine->AttachToComponent(
            &HandMesh,
            FAttachmentTransformRules::KeepWorldTransform,
            Context.Definition.MagazineHandBoneName);
        return Magazine;
    }

    void UpdateMagazineVisual(FBBBRifleUpdateContext &Context, FBBBRifleAnimationState &State)
    {
        const FBBBRifleActionState &Action = Context.RuntimeData.Action.ReadRifleActionState();
        if (!Action.bIsReloading)
        {
            ClearMagazineVisual(Context, State);
            return;
        }

        USkeletalMeshComponent *HandMesh = Context.Character.GetMesh();
        if (!ensureMsgf(Context.Definition.MagazineMesh && HandMesh
            && Context.WeaponMesh.GetBoneIndex(Context.Definition.MagazineBoneName) != INDEX_NONE
            && HandMesh->GetBoneIndex(Context.Definition.MagazineHandBoneName) != INDEX_NONE,
            TEXT("步枪换弹缺少弹匣网格或抓取骨骼 %s"), *Context.Equipment.GetName()))
        {
            return;
        }

        const FTransform HandWorld = HandMesh->GetSocketTransform(Context.Definition.MagazineHandBoneName);
        if (Action.bMagazineDetached && !State.bWasMagazineDetached)
        {
            // 先复制枪上弹匣的世界姿势 再隐藏骨骼 避免交接帧出现空档
            if (!Action.bMagazineReleased)
            {
                const FTransform MagazineWorld = Context.WeaponMesh.GetSocketTransform(Context.Definition.MagazineBoneName);
                State.HandMagazine = CreateHandMagazine(Context, *HandMesh, MagazineWorld);
                if (!State.HandMagazine.IsValid())
                {
                    return;
                }
            }

            Context.WeaponMesh.HideBoneByName(Context.Definition.MagazineBoneName, EPhysBodyOp::PBO_None);
            UE_LOG(LogTemp, Log, TEXT("[BBBRifle] Magazine detached Equipment=%s Sequence=%d"),
                *Context.Equipment.GetName(), Action.ReloadSequence);
        }

        if (Action.bMagazineReleased && !State.bWasMagazineReleased)
        {
            if (UStaticMeshComponent *HandMagazine = State.HandMagazine.Get())
            {
                // 仅控制端产生世界物理结果 镜像只收束当前手持外观
                if (!Context.Equipment.IsMirror())
                {
                    const FTransform ReleaseWorld = HandMagazine->GetComponentTransform();
                    FActorSpawnParameters SpawnParameters;
                    AStaticMeshActor *Dropped = Context.World.SpawnActor<AStaticMeshActor>(
                        ReleaseWorld.GetLocation(), ReleaseWorld.Rotator(), SpawnParameters);
                    if (ensureMsgf(Dropped, TEXT("步枪旧弹匣掉落 Actor 创建失败")))
                    {
                        UStaticMeshComponent *DroppedMesh = Dropped->GetStaticMeshComponent();
                        DroppedMesh->SetMobility(EComponentMobility::Movable);
                        DroppedMesh->SetStaticMesh(Context.Definition.MagazineMesh);
                        DroppedMesh->SetWorldTransform(ReleaseWorld);
                        DroppedMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
                        DroppedMesh->SetSimulatePhysics(true);

                        const float DeltaTime = FMath::Max(Context.World.GetDeltaSeconds(), KINDA_SMALL_NUMBER);
                        const FVector ThrowVelocity = State.bHasPreviousMagazineHandLocation
                            ? (HandWorld.GetLocation() - State.PreviousMagazineHandLocation) / DeltaTime
                            : Context.Character.GetVelocity();
                        DroppedMesh->SetPhysicsLinearVelocity(ThrowVelocity.GetClampedToMaxSize(1500.0f));
                        Dropped->SetLifeSpan(20.0f);
                    }
                }

                HandMagazine->DestroyComponent();
                State.HandMagazine.Reset();
            }

            UE_LOG(LogTemp, Log, TEXT("[BBBRifle] Magazine released Equipment=%s Sequence=%d"),
                *Context.Equipment.GetName(), Action.ReloadSequence);
        }

        if (Action.bFreshMagazineHeld && !State.bWasFreshMagazineHeld)
        {
            State.HandMagazine = CreateHandMagazine(
                Context,
                *HandMesh,
                Context.Definition.FreshMagazineHandTransform * HandWorld);
            if (!State.HandMagazine.IsValid())
            {
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("[BBBRifle] Fresh magazine taken Equipment=%s Sequence=%d"),
                *Context.Equipment.GetName(), Action.ReloadSequence);
        }

        State.bWasMagazineDetached = Action.bMagazineDetached;
        State.bWasMagazineReleased = Action.bMagazineReleased;
        State.bWasFreshMagazineHeld = Action.bFreshMagazineHeld;
        State.PreviousMagazineHandLocation = HandWorld.GetLocation();
        State.bHasPreviousMagazineHandLocation = State.HandMagazine.IsValid() && !Action.bFreshMagazineHeld;
    }
}

void FBBBRifleAnimationProcessor::Stop(FBBBRifleUpdateContext &Context)
{
    FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage, true);
    FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
    ClearMagazineVisual(Context, Context.RuntimeData.Animation.AnimationState);
}

void FBBBRifleAnimationProcessor::Update(FBBBRifleUpdateContext &Context)
{
    const auto &Action = Context.RuntimeData.Action.ReadRifleActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || !Context.Equipment.IsMirror()))
    {
        FBBBRiflePresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        ClearMagazineVisual(Context, State);
        FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBRiflePresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
    }

    if (!Action.bIsReloading && State.bIsReloading)
    {
        // 装匣成功后让人物收手和武器动作自然结束 中断才立刻停止
        if (!Action.bReloadCompletedThisFrame)
        {
            FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
            UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance();
            if (Animation && Context.Definition.EquipmentReloadMontage)
            {
                Animation->Montage_Stop(0.1f, Context.Definition.EquipmentReloadMontage);
            }
        }
    }

    UpdateMagazineVisual(Context, State);

    State.FireSequence = Action.FireSequence;
    State.ReloadSequence = Action.ReloadSequence;
    State.bIsReloading = Action.bIsReloading;
    State.bInitialized = true;

    UBBBRifleAnimInstance *Animation = Cast<UBBBRifleAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("步枪动画蓝图必须继承 UBBBRifleAnimInstance")))
    {
        return;
    }

    Animation->PublishAnimationFacts(State.Pose);
    Animation->PublishRifleSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds);
}
