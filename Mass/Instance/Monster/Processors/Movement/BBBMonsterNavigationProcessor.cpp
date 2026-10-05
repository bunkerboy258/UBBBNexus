#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"

UBBBMonsterNavigationProcessor::UBBBMonsterNavigationProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Decision);
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterBehaviorProcessor::StaticClass()->GetFName());
}

void UBBBMonsterNavigationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterNavigationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster navigation requires a valid world")))
    {
        return;
    }

    // 获取导航系统用于查询目标路径
    UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

    if (!ensureMsgf(NavigationSystem != nullptr, TEXT("[UBBBM]Monster navigation requires NavigationSystem")))
    {
        return;
    }

    // 使用处理器时间步长更新移动距离
    const float DeltaTime = Context.GetDeltaTimeSeconds();

    if (!ensureMsgf(DeltaTime >= 0.0f, TEXT("[UBBBM]Monster navigation delta time must not be negative")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();
    const int32 EntityCount = MonsterQuery.GetNumMatchingEntities();
    if (EntityCount == 0)
    {
        return;
    }

    constexpr int32 PathQueriesPerFrame = 32;
    const int32 FirstQueryIndex = NextPathQueryIndex % EntityCount;
    NextPathQueryIndex = (FirstQueryIndex + PathQueriesPerFrame) % EntityCount;
    int32 EntityIndex = 0;

    // 感知之后更新路径 位置 速度和朝向
    MonsterQuery.ForEachEntityChunk(Context, [DeltaTime, NavigationSystem, World, WorldTime,
        EntityCount, FirstQueryIndex, &EntityIndex](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        TArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetMutableFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FBBBMonsterTargetFragment> Targets = ChunkContext.GetFragmentView<FBBBMonsterTargetFragment>();
        TArrayView<FBBBMonsterMovementFragment> Movements = ChunkContext.GetMutableFragmentView<FBBBMonsterMovementFragment>();
        const TConstArrayView<FBBBMonsterBehaviorFragment> States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            const bool bMayQueryPath = ((EntityIndex++ - FirstQueryIndex + EntityCount) % EntityCount) < PathQueriesPerFrame;
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            FMassVelocityFragment& Velocity = Velocities[Index];
            const FBBBMonsterTargetFragment& Target = Targets[Index];
            FBBBMonsterMovementFragment& Movement = Movements[Index];
            const FBBBMonsterBehaviorFragment& State = States[Index];

            // 无目标或受控状态下停止移动
            if (!Target.bHasTarget || State.State != EBBBMonsterBehavior::Chase)
            {
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            const FVector CurrentLocation = Transform.GetLocation();
            FVector ToTarget = Target.TargetLocation - CurrentLocation;
            ToTarget.Z = 0.0f;

            const float StopRadius = FMath::Max(Movement.StopRadius, 0.0f);

            // 进入停止半径后交给战斗处理器
            if (ToTarget.SizeSquared() <= FMath::Square(StopRadius))
            {
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            if (bMayQueryPath && WorldTime >= Movement.NextPathRefreshTime)
            {
                // 按固定间隔重新查询到玩家的导航路径
                const UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(
                    World,
                    CurrentLocation,
                    Target.TargetLocation);

                if (Path != nullptr && Path->PathPoints.Num() > 1)
                {
                    Movement.NextPathPoint = Path->PathPoints[1];
                }
                else
                {
                    Movement.NextPathPoint = Target.TargetLocation;
                }

                Movement.NextPathRefreshTime = WorldTime + 0.25f;
            }

            if (Movement.NextPathRefreshTime <= 0.0f)
            {
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            FVector ToPathPoint = Movement.NextPathPoint - CurrentLocation;
            ToPathPoint.Z = 0.0f;

            if (ToPathPoint.IsNearlyZero())
            {
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            const FVector PathDirection = ToPathPoint.GetSafeNormal();
            const float MoveSpeed = FMath::Max(Movement.MoveSpeed, 0.0f);
            const float MoveDistance = FMath::Min3<double>(MoveSpeed * DeltaTime, ToPathPoint.Size(), FMath::Max(ToTarget.Size() - StopRadius, 0.0f));
            const FVector NewLocation = CurrentLocation + PathDirection * MoveDistance;

            // 逻辑层直接写入 Mass 变换 表现层随后读取该结果
            Transform.SetLocation(NewLocation);
            Transform.SetRotation(PathDirection.ToOrientationQuat());
            Velocity.Value = DeltaTime > SMALL_NUMBER ? PathDirection * (MoveDistance / DeltaTime) : FVector::ZeroVector;
        }
    });
}
