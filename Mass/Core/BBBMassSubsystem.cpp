#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"

#include "MassEntityConfigAsset.h"
#include "MassEntityTemplate.h"
#include "MassSpawnerSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Spawn/BBBProjectileSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"

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

bool UBBBMassSubsystem::QueryMonsterBodyPart(FMassEntityHandle Entity, uint8 Part, FBBBMonsterBodyPartDefinition& Result) const
{
    return UBBBMonsterDamageProcessor::QueryPart(*GetWorld(), Entity, Part, Result);
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBMonsterStateAuthorityFactPacket& Packet)
{
    return WriteInputSlot(*GetWorld(), Entity, Packet);
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBMonsterHitReactionLocalControlPacket& Packet)
{
    return WriteInputSlot(*GetWorld(), Entity, Packet);
}

void UBBBMassSubsystem::BeginCollisionFrame()
{
    CollisionCells.Reset();
}

bool UBBBMassSubsystem::RouteInput(FMassEntityHandle Entity, const FBBBMonsterSoundLocalControlPacket& Packet)
{
    if (GetWorld()->GetNetMode() == NM_Client)
    {
        return false;
    }
    return WriteInputSlot(*GetWorld(), Entity, Packet);
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
    FVector& HitPosition, FVector& HitNormal, EPhysicalSurface& HitSurface, uint8& HitPart) const
{
    HitTime = 1.0f;
    HitEntity.Reset();
    HitPosition = End;
    HitNormal = FVector::ZeroVector;
    HitSurface = SurfaceType_Default;
    HitPart = 0;
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
    TSet<FMassEntityHandle> CompoundCandidates;
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

                        double Time = C <= 0.0 ? 0.0 : (-B - FMath::Sqrt(Discriminant)) / LengthSquared;
                        FBBBMassCollisionBody Detailed;
                        const FBBBMassCollisionBody* Contact = &Body;
                        if (Body.bCompound)
                        {
                            if (Time < 0.0 || Time > HitTime || CompoundCandidates.Contains(Body.Entity))
                            {
                                continue;
                            }
                            CompoundCandidates.Add(Body.Entity);
                            float DetailedTime = 1.0f;
                            if (!UBBBMonsterCollisionProcessor::TraceCompound(*GetWorld(), Body.Entity, Start, End,
                                Radius, Detailed, DetailedTime))
                            {
                                continue;
                            }
                            Contact = &Detailed;
                            Time = DetailedTime;
                        }
                        if (Time >= 0.0 && Time <= HitTime)
                        {
                            HitTime = Time;
                            HitEntity = Body.Entity;
                            HitNormal = (Start + Delta * Time - Contact->Center).GetSafeNormal();
                            if (HitNormal.IsNearlyZero())
                            {
                                HitNormal = -Delta.GetSafeNormal();
                            }
                            HitPosition = Contact->Center + HitNormal * Contact->Radius;
                            HitSurface = Contact->Surface;
                            HitPart = Contact->Part;
                        }
                    }
                }
            }
        }
    }

    return HitEntity.IsSet();
}

void UBBBMassSubsystem::OverlapEntities(const FVector& Center, const float Radius,
    TArray<FBBBMassCollisionBody>& Results) const
{
    Results.Reset();
    if (Center.ContainsNaN() || !FMath::IsFinite(Radius) || Radius <= 0.0f)
    {
        return;
    }

    const FIntVector Min = CollisionCell(Center - FVector(Radius));
    const FIntVector Max = CollisionCell(Center + FVector(Radius));
    TMap<FMassEntityHandle, int32> Indices;
    TSet<FMassEntityHandle> CompoundCandidates;
    for (int32 X = Min.X; X <= Max.X; ++X)
    {
        for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
        {
            for (int32 Z = Min.Z; Z <= Max.Z; ++Z)
            {
                const auto* Bodies = CollisionCells.Find(FIntVector(X, Y, Z));
                if (Bodies == nullptr)
                {
                    continue;
                }

                for (const FBBBMassCollisionBody& Body : *Bodies)
                {
                    double Distance = FMath::Max(0.0, FVector::Distance(Center, Body.Center) - Body.Radius);
                    if (Distance > Radius)
                    {
                        continue;
                    }

                    FBBBMassCollisionBody Detailed;
                    const FBBBMassCollisionBody* Contact = &Body;
                    if (Body.bCompound)
                    {
                        if (CompoundCandidates.Contains(Body.Entity))
                        {
                            continue;
                        }
                        CompoundCandidates.Add(Body.Entity);
                        if (!UBBBMonsterCollisionProcessor::OverlapCompound(*GetWorld(), Body.Entity, Center, Radius, Detailed))
                        {
                            continue;
                        }
                        Contact = &Detailed;
                        Distance = FMath::Max(0.0, FVector::Distance(Center, Contact->Center) - Contact->Radius);
                    }

                    const int32* Index = Indices.Find(Body.Entity);
                    if (Index == nullptr)
                    {
                        Indices.Add(Body.Entity, Results.Add(*Contact));
                        continue;
                    }

                    const auto& Previous = Results[*Index];
                    const double PreviousDistance = FMath::Max(0.0, FVector::Distance(Center, Previous.Center) - Previous.Radius);
                    if (Distance < PreviousDistance)
                    {
                        Results[*Index] = *Contact;
                    }
                }
            }
        }
    }
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
