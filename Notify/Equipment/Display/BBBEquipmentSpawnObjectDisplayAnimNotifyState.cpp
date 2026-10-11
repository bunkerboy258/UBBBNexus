#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBEquipmentSpawnObjectDisplayAnimNotifyState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/BodySetup.h"

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
    const FVector WorldLinearVelocity = SocketTransform.TransformVectorNoScale(LinearVelocity) + InheritedVelocity;
    FActorSpawnParameters Parameters;
    Parameters.ObjectFlags |= RF_Transient;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Parameters.bDeferConstruction = true;
    AStaticMeshActor *Actor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Parameters);
    if (!ensureMsgf(Actor, TEXT("装备动画物体创建失败 %s"), *ObjectMesh->GetPathName()))
    {
        return;
    }

    UStaticMeshComponent *Component = Actor->GetStaticMeshComponent();
    Component->SetMobility(EComponentMobility::Movable);
    Component->SetStaticMesh(ObjectMesh);
    Component->SetCollisionObjectType(ECC_PhysicsBody);
    Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Component->SetCollisionResponseToAllChannels(ECR_Ignore);
    Component->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Actor->FinishSpawning(Transform);
    Actor->SetLifeSpan(LifeSeconds);
    Component->SetSimulatePhysics(true);
    if (!ensureMsgf(Component->IsSimulatingPhysics(), TEXT("装备动画物体无法开启物理模拟 %s"), *ObjectMesh->GetPathName()))
    {
        Actor->Destroy();
        return;
    }

    // 只在脱离武器时继承移动速度 后续由独立物理模拟决定轨迹
    Component->SetPhysicsLinearVelocity(WorldLinearVelocity);
    Component->SetPhysicsAngularVelocityInDegrees(SocketTransform.TransformVectorNoScale(AngularVelocityDegrees));
}
