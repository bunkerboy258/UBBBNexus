#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Animation/BBBMaodieTestAnimInstanceProxy.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Animation/BBBMaodieTestAnimInstance.h"
#include "BonePose.h"
FBBBMaodieTestAnimInstanceProxy::FBBBMaodieTestAnimInstanceProxy(UAnimInstance *Instance)
    : FAnimSingleNodeInstanceProxy(Instance)
{
}
void FBBBMaodieTestAnimInstanceProxy::PreUpdate(UAnimInstance *Instance, float DeltaSeconds)
{
    FAnimSingleNodeInstanceProxy::PreUpdate(Instance, DeltaSeconds);
    HeadRotation = CastChecked<UBBBMaodieTestAnimInstance>(Instance)->HeadRotation;
}
bool FBBBMaodieTestAnimInstanceProxy::Evaluate(FPoseContext &Output)
{
    const bool bEvaluated = FAnimSingleNodeInstanceProxy::Evaluate(Output);
    if (!bEvaluated)
    {
        return false;
    }
    const auto &Bones = Output.Pose.GetBoneContainer();
    const int32 MeshIndex = Bones.GetReferenceSkeleton().FindBoneIndex(TEXT("head"));
    if (MeshIndex == INDEX_NONE)
    {
        return true;
    }
    const auto HeadIndex = Bones.MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshIndex));
    if (HeadIndex.GetInt() == INDEX_NONE)
    {
        return true;
    }
    const auto ParentIndex = Bones.GetParentBoneIndex(HeadIndex);
    if (ParentIndex.GetInt() == INDEX_NONE)
    {
        return true;
    }
    FCSPose<FCompactPose> ComponentPose;
    ComponentPose.InitPose(Output.Pose);
    const FQuat ParentRotation = ComponentPose.GetComponentSpaceTransform(ParentIndex).GetRotation();
    //源网格朝向本地正 Y 轴 抬头使用本地正 X 轴
    const FQuat Offset = FQuat(FVector::UpVector, FMath::DegreesToRadians(HeadRotation.Yaw))
        * FQuat(FVector::ForwardVector, FMath::DegreesToRadians(HeadRotation.Pitch));
    FTransform &Head = Output.Pose[HeadIndex];
    Head.SetRotation((ParentRotation.Inverse() * Offset * ParentRotation * Head.GetRotation()).GetNormalized());
    return true;
}
