#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"

#include "MassEntityConfigAsset.h"
#include "MassEntityTemplate.h"
#include "MassSpawnerSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"

namespace
{
    constexpr float CollisionCellSize = 250.0f;

    template<typename TPacket>
    bool WriteInputSlot(UWorld& World, FMassEntityHandle Entity, const TPacket& Packet)
    {
        UMassEntitySubsystem* Entities = World.GetSubsystem<UMassEntitySubsystem>();
        if (!ensureMsgf(IsInGameThread() && Entities != nullptr, TEXT("Mass 输入必须在游戏线程投递")))
        {
            return false;
        }

        FMassEntityManager& Manager = Entities->GetMutableEntityManager();
        if (!Manager.IsEntityValid(Entity))
        {
            return false;
        }

        auto* Slots = Manager.GetFragmentDataPtr<typename TPacket::FInputFragment>(Entity);
        if (!ensureMsgf(Slots != nullptr, TEXT("Mass 目标缺少对应输入片段")))
        {
            return false;
        }

        auto& Slot = Slots->template GetSlot<TPacket>();
        Slot.Packet = Packet;
        Slot.bActive = true;
        return true;
    }

    FIntVector CollisionCell(const FVector& Position)
    {
        return FIntVector(
            FMath::FloorToInt(Position.X / CollisionCellSize),
            FMath::FloorToInt(Position.Y / CollisionCellSize),
            FMath::FloorToInt(Position.Z / CollisionCellSize));
    }
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBProjectileSpawnLocalControlPacket& Packet)
{
    return WriteInputSlot(*GetWorld(), Entity, Packet);
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBMonsterDamageLocalControlPacket& Packet)
{
    return WriteInputSlot(*GetWorld(), Entity, Packet);
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBMonsterDamageRemoteMessagePacket& Packet)
{
    return WriteInputSlot(*GetWorld(), Entity, Packet);
}

bool UBBBMassSubsystem::QueryDamage(FMassEntityHandle Entity, TArray<FBBBMonsterDamageContribution>& Result) const
{
    return UBBBMonsterDamageProcessor::Query(*GetWorld(), Entity, Result);
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBMonsterStateAuthorityFactPacket& Packet)
{
    return WriteInputSlot(*GetWorld(), Entity, Packet);
}

void UBBBMassSubsystem::BeginCollisionFrame()
{
    CollisionCells.Reset();
}

void UBBBMassSubsystem::AddCollisionBody(const FBBBMassCollisionBody& Body)
{
    const FIntVector Min = CollisionCell(Body.Center - FVector(Body.Radius));
    const FIntVector Max = CollisionCell(Body.Center + FVector(Body.Radius));
    for (int32 X = Min.X; X <= Max.X; ++X)
    {
        for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
        {
            for (int32 Z = Min.Z; Z <= Max.Z; ++Z)
            {
                CollisionCells.FindOrAdd(FIntVector(X, Y, Z)).Add(Body);
            }
        }
    }
}

bool UBBBMassSubsystem::TraceEntities(const FVector& Start, const FVector& End, const float Radius,
    TConstArrayView<FMassEntityHandle> Ignored, FMassEntityHandle& HitEntity, float& HitTime,
    FVector& HitPosition, FVector& HitNormal, EPhysicalSurface& HitSurface) const
{
    HitTime = 1.0f;
    HitEntity.Reset();
    HitPosition = End;
    HitNormal = FVector::ZeroVector;
    HitSurface = SurfaceType_Default;
    const FVector Delta = End - Start;
    const double LengthSquared = Delta.SizeSquared();
    if (LengthSquared <= UE_SMALL_NUMBER)
    {
        return false;
    }

    // 按线段穿过的采样单元查询邻近桶 避免斜向长线段遍历整个包围盒体积
    const int32 Steps = FMath::Max(1, FMath::CeilToInt(Delta.GetAbsMax() / CollisionCellSize));
    const int32 Expansion = FMath::Max(1, FMath::CeilToInt(Radius / CollisionCellSize));
    TSet<FIntVector> Visited;
    for (int32 Step = 0; Step <= Steps; ++Step)
    {
        const FIntVector Cell = CollisionCell(Start + Delta * (static_cast<double>(Step) / Steps));
        for (int32 X = -Expansion; X <= Expansion; ++X)
        {
            for (int32 Y = -Expansion; Y <= Expansion; ++Y)
            {
                for (int32 Z = -Expansion; Z <= Expansion; ++Z)
                {
                    const FIntVector Key = Cell + FIntVector(X, Y, Z);
                    if (Visited.Contains(Key))
                    {
                        continue;
                    }

                    Visited.Add(Key);
                    const auto* Bodies = CollisionCells.Find(Key);
                    if (Bodies == nullptr)
                    {
                        continue;
                    }

                    for (const FBBBMassCollisionBody& Body : *Bodies)
                    {
                        if (Ignored.Contains(Body.Entity))
                        {
                            continue;
                        }

                        const FVector Offset = Start - Body.Center;
                        const double CombinedRadius = Radius + Body.Radius;
                        const double C = Offset.SizeSquared() - CombinedRadius * CombinedRadius;
                        const double B = FVector::DotProduct(Offset, Delta);
                        const double Discriminant = B * B - LengthSquared * C;
                        if (Discriminant < 0.0)
                        {
                            continue;
                        }

                        const double Time = C <= 0.0 ? 0.0 : (-B - FMath::Sqrt(Discriminant)) / LengthSquared;
                        if (Time >= 0.0 && Time <= HitTime)
                        {
                            HitTime = Time;
                            HitEntity = Body.Entity;
                            HitNormal = (Start + Delta * Time - Body.Center).GetSafeNormal();
                            if (HitNormal.IsNearlyZero())
                            {
                                HitNormal = -Delta.GetSafeNormal();
                            }
                            HitPosition = Body.Center + HitNormal * Body.Radius;
                            HitSurface = Body.Surface;
                        }
                    }
                }
            }
        }
    }

    return HitEntity.IsSet();
}

FMassEntityHandle UBBBMassSubsystem::CreateEntity(const UMassEntityConfigAsset& Config)
{
    UMassSpawnerSubsystem* Spawner = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
    if (!ensureMsgf(IsInGameThread() && Spawner != nullptr, TEXT("Mass 创建需要游戏线程与生成子系统")))
    {
        return {};
    }

    // 调度期间禁止直接改变实体结构 调用方必须在安全阶段完成创建
    if (!ensureMsgf(!Spawner->GetEntityManagerChecked().IsProcessing(), TEXT("Mass 创建不能发生在查询执行期间")))
    {
        return {};
    }

    const FMassEntityTemplate& Template = Config.GetOrCreateEntityTemplate(*GetWorld());
    if (!ensureMsgf(Template.IsValid(), TEXT("Mass 实体模板无效 %s"), *Config.GetPathName()))
    {
        return {};
    }

    TArray<FMassEntityHandle> Entities;
    auto Creation = Spawner->SpawnEntities(Template, 1, Entities);
    if (!ensureMsgf(Creation.IsValid() && Entities.Num() == 1, TEXT("Mass 创建未返回唯一实体")))
    {
        return {};
    }

    return Entities[0];
}
