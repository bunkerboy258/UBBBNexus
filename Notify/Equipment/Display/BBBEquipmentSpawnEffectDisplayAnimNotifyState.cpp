#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBEquipmentSpawnEffectDisplayAnimNotifyState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UBBBEquipmentSpawnEffectDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    if (!Effect)
    {
        return;
    }

    UWorld *World = MeshComp ? MeshComp->GetWorld() : nullptr;
    if (!ensureMsgf(IsInGameThread() && IsValid(MeshComp) && World
        && (SocketName.IsNone() || MeshComp->DoesSocketExist(SocketName))
        && !RelativeTransform.ContainsNaN() && !Effect->IsLooping(),
        TEXT("装备生成特效通知缺少有效网格 插槽 变换或使用了循环特效 %s"), *GetPathName()))
    {
        return;
    }

    if (World->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    UNiagaraComponent *Component = nullptr;
    if (bAttached)
    {
        Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
            Effect, MeshComp, SocketName,
            RelativeTransform.GetLocation(), RelativeTransform.Rotator(), RelativeTransform.GetScale3D(),
            EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::AutoRelease, true, false);
    }

    if (!bAttached)
    {
        const FTransform Transform = RelativeTransform * MeshComp->GetSocketTransform(SocketName);
        Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World, Effect, Transform.GetLocation(), Transform.Rotator(), Transform.GetScale3D(),
            true, true, ENCPoolMethod::AutoRelease, false);
    }

    ensureMsgf(Component, TEXT("装备动画特效生成失败 %s"), *Effect->GetPathName());
}
