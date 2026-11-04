#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBEquipmentSpawnObjectDisplayAnimNotifyState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/BodySetup.h"
#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBEquipmentDisplaySubsystem.h"

void UBBBEquipmentSpawnObjectDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    if (!ObjectMesh)
    {
        return;
    }

    UWorld *World = MeshComp ? MeshComp->GetWorld() : nullptr;
    const UBodySetup *BodySetup = ObjectMesh->GetBodySetup();
    const FVector Scale = RelativeTransform.GetScale3D();
    if (!ensureMsgf(IsInGameThread() && IsValid(MeshComp) && World
        && (SocketName.IsNone() || MeshComp->DoesSocketExist(SocketName))
        && !RelativeTransform.ContainsNaN() && Scale.X > 0.0 && Scale.Y > 0.0 && Scale.Z > 0.0
        && !LinearVelocity.ContainsNaN() && !AngularVelocityDegrees.ContainsNaN()
        && FMath::IsFinite(VelocitySpreadDegrees) && VelocitySpreadDegrees >= 0.0f && VelocitySpreadDegrees <= 90.0f
        && FMath::IsFinite(LifeSeconds) && LifeSeconds > 0.0f
        && BodySetup && BodySetup->AggGeom.GetElementCount() > 0,
        TEXT("装备生成物体通知缺少有效网格 插槽 简单碰撞 变换 速度或存在时间 %s"), *GetPathName()))
    {
        return;
    }

    if (World->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
    const FTransform Transform = RelativeTransform * SocketTransform;
    // 只读取直接目标装备发布的移动速度 避免跨对象访问持有角色
    const AActor *Equipment = MeshComp->GetOwner();
    const FVector InheritedVelocity = IsValid(Equipment) ? Equipment->GetVelocity() : FVector::ZeroVector;
    FVector LocalLinearVelocity = LinearVelocity;
    if (VelocitySpreadDegrees > 0.0f && !LinearVelocity.IsNearlyZero())
    {
        LocalLinearVelocity = FMath::VRandCone(LinearVelocity.GetSafeNormal(), FMath::DegreesToRadians(VelocitySpreadDegrees)) * LinearVelocity.Size();
    }
    const FVector WorldLinearVelocity = SocketTransform.TransformVectorNoScale(LocalLinearVelocity) + InheritedVelocity;
    if (auto *Display = World->GetSubsystem<UBBBEquipmentDisplaySubsystem>())
    {
        Display->Emit(ObjectMesh, Transform, WorldLinearVelocity,
            SocketTransform.TransformVectorNoScale(AngularVelocityDegrees) * (PI / 180.0), LifeSeconds, true);
    }
}
