#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Presentation/BBBProjectilePresentationProcessor.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassSimulationSubsystem.h"
#include "MassProcessingPhaseManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Misc/App.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"
UBBBProjectilePresentationProcessor::UBBBProjectilePresentationProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::FrameEnd;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Collision);
}

void UBBBProjectilePresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBProjectilePresentationFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileLifetimeFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBProjectilePresentationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }

    FreeSlots.Append(PendingReleaseSlots);
    PendingReleaseSlots.Reset();

    TArray<FTransformFragment> Transforms;
    TArray<FBBBProjectileMotionFragment> Motion;
    TArray<FBBBProjectilePresentationFragment> Presentation;

    EntityQuery.ForEachEntityChunk(Context, [this, World, &Transforms, &Motion, &Presentation](FMassExecutionContext& Chunk)
    {
        const auto ChunkTransforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto ChunkMotion = Chunk.GetFragmentView<FBBBProjectileMotionFragment>();
        auto ChunkPresentation = Chunk.GetMutableFragmentView<FBBBProjectilePresentationFragment>();
        const auto ChunkLife = Chunk.GetFragmentView<FBBBProjectileLifetimeFragment>();

        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Visual = ChunkPresentation[Index];
            const bool bEnding = ChunkLife[Index].bPendingDestroy
                || ChunkLife[Index].RemainingSeconds <= Chunk.GetDeltaTimeSeconds();
            if (!ChunkMotion[Index].bInitialized || !Visual.Channel.IsValid() || !Visual.System.IsValid())
            {
                continue;
            }

            if (ActiveChannel.IsValid() && ActiveChannel != Visual.Channel)
            {
                ensureMsgf(false, TEXT("同一世界的子弹表现必须使用同一个 Niagara 数据通道"));
                continue;
            }

            if (ActiveSystem.IsValid() && ActiveSystem != Visual.System)
            {
                ensureMsgf(false, TEXT("同一世界的子弹表现必须使用同一个 Niagara 系统"));
                continue;
            }

            ActiveChannel = Visual.Channel;
            ActiveSystem = Visual.System;

            if (bEnding && Visual.Slot == INDEX_NONE
                && ChunkTransforms[Index].GetTransform().GetLocation().Equals(ChunkMotion[Index].SpawnLocation, UE_SMALL_NUMBER))
            {
                continue;
            }

            if (SystemComponent == nullptr)
            {
                UMassSimulationSubsystem* Simulation = World->GetSubsystem<UMassSimulationSubsystem>();
                if (!ensureMsgf(Simulation != nullptr && Simulation->IsSimulationStarted()
                    && Visual.System->bRequireCurrentFrameData,
                    TEXT("共享子弹光效需要运行中的 Mass 调度与当帧模拟配置")))
                {
                    continue;
                }

                // 仅创建一个世界级 Niagara 组件 所有 Mass 子弹共享这次模拟
                SystemComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    World, Visual.System.Get(), FVector::ZeroVector, FRotator::ZeroRotator,
                    FVector::OneVector, false, false, ENCPoolMethod::None, false);
                if (!ensureMsgf(SystemComponent != nullptr, TEXT("无法启动共享子弹曳光系统")))
                {
                    continue;
                }

                // 共享组件脱离普通光效批次 单独等待帧末 Mass 完成后读取当帧位置
                SystemComponent->SetForceSolo(true);
                SystemComponent->SetTickBehavior(ENiagaraTickBehavior::ForceTickLast);
                SystemComponent->PrimaryComponentTick.AddPrerequisite(Simulation,
                    Simulation->GetMutablePhaseManager().GetProcessingPhaseTickFunction(EMassProcessingPhase::FrameEnd));
                SystemComponent->Activate();
            }

            if (Visual.Slot == INDEX_NONE)
            {
                Visual.Slot = FreeSlots.IsEmpty() ? NextSlot++ : FreeSlots.Pop(EAllowShrinking::No);
                Visual.bSpawnPending = true;
            }

            if (Visual.Slot == INDEX_NONE)
            {
                continue;
            }

            // 碰撞帧发布消亡事实 槽位延至下帧回收 避免新子弹覆盖旧光段
            Visual.bVisualAlive = !bEnding;
            Transforms.Add(ChunkTransforms[Index]);
            Motion.Add(ChunkMotion[Index]);
            Presentation.Add(Visual);
            Visual.bSpawnPending = false;

            if (!Visual.bVisualAlive)
            {
                PendingReleaseSlots.Add(Visual.Slot);
                Visual.Slot = INDEX_NONE;
            }
        }
    });

    if (!Presentation.IsEmpty())
    {
        FBBBProjectilePresentation::Publish(*World, Transforms, Motion, Presentation);
    }
}
