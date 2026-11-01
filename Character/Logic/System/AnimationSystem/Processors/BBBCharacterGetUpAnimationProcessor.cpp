#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterGetUpAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "PoseSearch/PoseSearchHistory.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "PoseSearch/PoseSearchResult.h"

namespace
{
/** 将已经完成物理混合的网格姿态提供给标准姿态搜索接口 */
class FGetUpPoseProvider final : public UE::PoseSearch::IComponentSpacePoseProvider
{
public:
    /** @param InMesh 当前身体网格 */
    explicit FGetUpPoseProvider(const USkeletalMeshComponent &InMesh) : Mesh(InMesh) {}
    const USkeleton *GetSkeletonAsset() const override { return Mesh.GetSkeletalMeshAsset()->GetSkeleton(); }
    FTransform CalculateComponentSpaceTransform(FSkeletonPoseBoneIndex Index) override
    {
        return Mesh.GetSocketTransform(GetSkeletonAsset()->GetReferenceSkeleton().GetBoneName(Index.GetInt()), RTS_Component);
    }
private:
    const USkeletalMeshComponent &Mesh;
};
}

void FBBBCharacterGetUpAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.GetUpAnimationState;
    const auto &Physical = Context.RuntimeData.PhysicalPresentation.ReadPhysicalPresentationState();
    const auto &Result = Physical.RagdollSnapshot;
    auto &Anim = Context.AnimationInstance;
    auto &Mesh = Context.CharacterMesh;
    if (Physical.bRagdoll)
    {
        // 保留真实物理姿态 选片不读取停留在进入布娃娃之前的运动历史
        if (!Physical.bCanGetUp || !Physical.bGetUpRequested)
        {
            State.SelectedEpisode = 0;
            State.SelectedMontage = nullptr;
        }
        if (Physical.bGetUpRequested && Physical.bCanGetUp
            && State.SelectedEpisode != Result.Episode)
        {
            UPoseSearchDatabase *Database = Context.AnimationConfig.GetUpStandDatabase.Get();
            if (!ensureMsgf(Database, TEXT("起身搜索库尚未配置")))
            {
                return;
            }
            UE::PoseSearch::FPoseHistory History;
            History.Initialize_AnyThread(2, 0.0f);
            FGetUpPoseProvider Provider(Mesh);
            TArray<FBoneIndexType> Bones;
            const int32 Count = Provider.GetSkeletonAsset()->GetReferenceSkeleton().GetNum();
            for (int32 Index = 0; Index < Count; ++Index)
            {
                Bones.Add(static_cast<FBoneIndexType>(Index));
            }
            History.EvaluateComponentSpace_AnyThread(0.0f, Provider, false, 0.0f, 0.0f, 0.0f, true, true, Bones);
            const UObject *Contexts[] = {&Anim};
            const UE::PoseSearch::FRole Roles[] = {UE::PoseSearch::DefaultRole};
            const UE::PoseSearch::IPoseHistory *Histories[] = {&History};
            const UObject *Assets[] = {Database};
            const auto Match = UPoseSearchLibrary::MotionMatch(Contexts, Roles, Histories, Assets,
                FPoseSearchContinuingProperties(), FPoseSearchFutureProperties(), FPoseSearchEvent());
            if (Match.IsValid())
            {
                auto *Sequence = const_cast<UAnimSequence *>(Cast<UAnimSequence>(Match.GetCurrentResultAnimationAsset()));
                const auto *Montage = Context.AnimationConfig.GetUpMontages.Find(Sequence);
                if (Montage && *Montage)
                {
                    State.SelectedMontage = *Montage;
                    State.SelectedTime = Match.GetAssetTime();
                    State.SelectedEpisode = Result.Episode;
                }
            }
        }
    }
    if (Result.Phase != EBBBCharacterPhysicalPhase::Recovering && State.bRootMotionOverride)
    {
        Anim.SetRootMotionMode(static_cast<ERootMotionMode::Type>(State.PreviousRootMotionMode));
        State.bRootMotionOverride = false;
    }
    if (Result.Transition == State.AppliedTransition)
    {
        if (Result.Phase == EBBBCharacterPhysicalPhase::Recovering && Result.GetUpMontage
            && Anim.Montage_IsActive(Result.GetUpMontage)
            && FMath::Abs(Anim.Montage_GetPosition(Result.GetUpMontage) - Result.GetUpPosition) > 0.2f)
        {
            Anim.Montage_SetPosition(Result.GetUpMontage, Result.GetUpPosition);
        }
        return;
    }
    if (Result.Phase != EBBBCharacterPhysicalPhase::Recovering)
    {
        if (Result.Phase == EBBBCharacterPhysicalPhase::Animated && State.RecoveryPose.bIsValid)
        {
            // 起身完成通过已有退出动作覆盖固定全身槽 不直接停止蒙太奇
            if (ensureMsgf(Context.AnimationConfig.LifeActionReturnMontage, TEXT("起身缺少退出动作")))
            {
                Anim.RegisterMontageContribution(BBBCharacterMontageSlots::FullBody,
                    Context.AnimationConfig.LifeActionReturnMontage);
            }
        }
        State.AppliedTransition = Result.Transition;
        return;
    }
    if (!ensureMsgf(Result.GetUpMontage, TEXT("起身阶段缺少已选定动作")))
    {
        return;
    }
    State.RecoveryPose = Physical.ExitPose;
    State.CaptureMeshTransform = Physical.RagdollMeshTransform;
    if (State.RecoveryPose.bIsValid && !State.RecoveryPose.LocalTransforms.IsEmpty())
    {
        // 网格重新挂回胶囊之后只重定位快照根变换 保持所有物理骨骼世界姿态连续
        State.RecoveryPose.LocalTransforms[0] = State.RecoveryPose.LocalTransforms[0]
            * State.CaptureMeshTransform.GetRelativeTransform(Mesh.GetComponentTransform());
    }
    if (!State.bRootMotionOverride)
    {
        State.PreviousRootMotionMode = static_cast<uint8>(Anim.RootMotionMode);
        State.bRootMotionOverride = true;
    }
    Anim.SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
    if (Anim.RegisterMontageContribution(BBBCharacterMontageSlots::FullBody, Result.GetUpMontage))
    {
        Anim.Montage_SetPosition(Result.GetUpMontage, Result.GetUpPosition);
        State.AppliedTransition = Result.Transition;
    }
}
