#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterVisualizationLODProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassLODSubsystem.h"
#include "MassRepresentationFragments.h"

UBBBMonsterVisualizationLODProcessor::UBBBMonsterVisualizationLODProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UBBBMonsterVisualizationLODProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // 为近处 远处和调试查询统一添加小怪标签
    Super::ConfigureQueries(EntityManager);

    CloseEntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    CloseEntityAdjustDistanceQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    FarEntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    DebugEntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    // 记录筛选标签供可视化查询使用
    FilterTag = FBBBMonsterTag::StaticStruct();
}

void UBBBMonsterVisualizationLODProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    Super::Execute(EntityManager, Context);

    const UWorld* World = Context.GetWorld();
    if (!World || World->GetNetMode() != NM_ListenServer)
    {
        return;
    }

    struct FLocalView
    {
        FVector Location;
        FVector Forward;
    };
    TArray<FLocalView, TInlineAllocator<2>> LocalViews;
    // 复用引擎已采集的视角快照 避免在 LOD 处理线程读取摄像机状态
    const UMassLODSubsystem& LODSubsystem = Context.GetSubsystemChecked<UMassLODSubsystem>();
    for (const FViewerInfo& Viewer : LODSubsystem.GetViewers())
    {
        const APlayerController* Controller = Viewer.GetPlayerController();
        if (Viewer.Handle.IsValid() && Viewer.bEnabled && Controller && Controller->GetLocalPlayer())
        {
            LocalViews.Add({Viewer.Location, Viewer.Rotation.Vector()});
        }
    }
    if (LocalViews.IsEmpty())
    {
        return;
    }

    CloseEntityQuery.ForEachEntityChunk(Context, [&LocalViews](FMassExecutionContext& Chunk)
    {
        constexpr double CullDistance = 5000.0;
        constexpr double BehindViewMargin = 1000.0;
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        auto Levels = Chunk.GetMutableFragmentView<FMassRepresentationLODFragment>();
        for (int32 Index = 0; Index < Levels.Num(); ++Index)
        {
            auto& Level = Levels[Index];
            if (Level.LOD >= EMassLOD::Off)
            {
                continue;
            }
            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const bool bOutsideLocalViews = !LocalViews.ContainsByPredicate([&Location](const FLocalView& View)
            {
                const FVector Offset = Location - View.Location;
                return Offset.SizeSquared() < FMath::Square(CullDistance)
                    || FVector::DotProduct(Offset, View.Forward) >= -BehindViewMargin;
            });
            if (bOutsideLocalViews)
            {
                // 只改变主机本地表现 远端根据自己的视角继续选择表现
                Level.LOD = EMassLOD::Off;
                Level.LODSignificance = static_cast<float>(EMassLOD::Off);
                Level.Visibility = EMassVisibility::CulledByFrustum;
            }
        }
    });
}
