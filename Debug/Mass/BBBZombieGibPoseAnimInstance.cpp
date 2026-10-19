#include "BBBZombieGibPoseAnimInstance.h"

#include "Animation/AnimInstanceProxy.h"
#include "Components/SkeletalMeshComponent.h"

namespace
{
    /** 当前姿态在游戏线程复制到代理 不读取其它对象的动画状态 */
    struct FZombieGibPoseProxy final : FAnimInstanceProxy
    {
        TArray<FTransform> Local;

        explicit FZombieGibPoseProxy(UAnimInstance* Instance) : FAnimInstanceProxy(Instance) {}

        virtual void PreUpdate(UAnimInstance* Instance, float DeltaSeconds) override
        {
            FAnimInstanceProxy::PreUpdate(Instance, DeltaSeconds);
            Local = CastChecked<UBBBZombieGibPoseAnimInstance>(Instance)->Pose.LocalTransforms;
        }

        virtual bool Evaluate(FPoseContext& Output) override
        {
            Output.ResetToRefPose();
            const auto& Bones = Output.Pose.GetBoneContainer();
            for (const FCompactPoseBoneIndex Index : Output.Pose.ForEachBoneIndex())
            {
                const int32 MeshIndex = Bones.MakeMeshPoseIndex(Index).GetInt();
                if (Local.IsValidIndex(MeshIndex))
                {
                    Output.Pose[Index] = Local[MeshIndex];
                }
            }
            return true;
        }
    };
}

void UBBBZombieGibPoseAnimInstance::Capture(USkeletalMeshComponent& Source)
{
    Source.SnapshotPose(Pose);
}

FAnimInstanceProxy* UBBBZombieGibPoseAnimInstance::CreateAnimInstanceProxy()
{
    return new FZombieGibPoseProxy(this);
}

void UBBBZombieGibPoseAnimInstance::DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy)
{
    delete Proxy;
}
