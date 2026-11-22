#include "BBBMonsterDirectorPresentationActor.h"
#include "GameFramework/PlayerState.h"
#include "BBBMonsterSpawnRegionPresentationActor.h"
#include "Components/SceneComponent.h"
#include "MassEntityConfigAsset.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterDirectorTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Director/BBBMonsterDirectorParseProcessor.h"

ABBBMonsterDirectorPresentationActor::ABBBMonsterDirectorPresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DirectorRoot"));
}

bool ABBBMonsterDirectorPresentationActor::ValidateConfiguration(FString& Error) const
{
    if (!Definition || !EntityConfig || !Definition->Validate(Error))
    {
        Error = TEXT("导演定义或实体模板无效 ") + Error;
        return false;
    }
    if (EntityConfig->GetConfig().GetTraits().Num() != 1
        || !EntityConfig->GetConfig().FindTrait(UBBBMonsterDirectorTrait::StaticClass(), true))
    {
        Error = TEXT("导演实体模板必须仅装配导演 Trait");
        return false;
    }
    TSet<FName> Seen;
    for (const ABBBMonsterSpawnRegionPresentationActor* Region : Regions)
    {
        if (!IsValid(Region) || Region->GetWorld() != GetWorld())
        {
            Error = TEXT("生成区域为空或不属于当前地图");
            return false;
        }
        const auto Config = Region->ReadDefinition();
        if (!Config.IsValid() || Seen.Contains(Config.Id))
        {
            Error = TEXT("生成区域标识重复或尺寸无效");
            return false;
        }
        if (Config.Role == EBBBMonsterSpawnRegionRole::Population
            && !Config.SpawnDefinitionOverride.IsValid() && !Definition->PopulationSpawnDefinition)
        {
            Error = FString::Printf(TEXT("固有区域 %s 缺少出生配置且导演未设置默认固有出生配置"), *Config.Id.ToString());
            return false;
        }
        Seen.Add(Config.Id);
    }
    for (const auto& Wave : Definition->EventWaves)
    {
        for (const FName Id : Wave.RegionIds)
        {
            if (!Seen.Contains(Id))
            {
                Error = FString::Printf(TEXT("预制波次 %s 缺少地图区域 %s"), *Wave.EventId.ToString(), *Id.ToString());
                return false;
            }
        }
    }
    Error.Reset();
    return true;
}

bool ABBBMonsterDirectorPresentationActor::EnsureEntity()
{
    if (!HasAuthority() || !GetWorld() || GetWorld()->GetNetMode() == NM_Client || !GetWorld()->IsGameWorld() || !EntityConfig)
    {
        return false;
    }
    UMassEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    if (Entities && Entities->GetEntityManager().IsEntityValid(Entity))
    {
        return true;
    }
    auto* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    if (!Mass)
    {
        return false;
    }
    FString Error;
    if (!ValidateConfiguration(Error))
    {
        UE_LOG(LogTemp, Error, TEXT("[BBBMonsterDirector] %s %s"), *GetName(), *Error);
        return false;
    }
    Entity = Mass->CreateEntity(*EntityConfig);
    if (!Entity.IsSet())
    {
        return false;
    }
    FBBBMonsterDirectorInitializeLocalControlPacket Packet;
    Packet.InstanceId = FGuid::NewGuid();
    Packet.Seed = static_cast<uint32>(FMath::Rand()) | 1u;
    Packet.Definition = Definition.Get();
    Packet.Presentation = this;
    for (const ABBBMonsterSpawnRegionPresentationActor* Region : Regions)
    {
        Packet.Regions.Add(Region->ReadDefinition());
    }
    Mass->SubmitInput(Entity, MoveTemp(Packet));
    return true;
}

void ABBBMonsterDirectorPresentationActor::BeginPlay()
{
    Super::BeginPlay();
    if (EnsureEntity() && bAutoStart)
    {
        StartDirector();
    }
}

void ABBBMonsterDirectorPresentationActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (auto* Mass = GetWorld() ? GetWorld()->GetSubsystem<UBBBMassSubsystem>() : nullptr)
    {
        Mass->DestroyEntity(Entity);
    }
    Entity.Reset();
    Super::EndPlay(EndPlayReason);
}

FBBBMonsterDirectorStatus ABBBMonsterDirectorPresentationActor::ReadStatus() const
{
    FBBBMonsterDirectorStatus Result;
    if (GetWorld())
    {
        UBBBMonsterDirectorParseProcessor::Query(*GetWorld(), Entity, Result);
    }
    if (!Result.InstanceId.IsValid())
    {
        ValidateConfiguration(Result.Reason);
    }
    return Result;
}

bool ABBBMonsterDirectorPresentationActor::StartDirector()
{
    return HasAuthority() && SubmitInput(FBBBMonsterDirectorStartLocalControlPacket());
}

bool ABBBMonsterDirectorPresentationActor::StopDirector()
{
    return HasAuthority() && SubmitInput(FBBBMonsterDirectorStopLocalControlPacket());
}

bool ABBBMonsterDirectorPresentationActor::SetDynamicHordePaused(const bool bPaused)
{
    FBBBMonsterDirectorSetDynamicHordePausedLocalControlPacket Packet;
    Packet.bPaused = bPaused;
    return HasAuthority() && SubmitInput(MoveTemp(Packet));
}

bool ABBBMonsterDirectorPresentationActor::TriggerEventWave(const FName EventId, APlayerState* InitialTarget)
{
    if (EventId.IsNone())
    {
        return false;
    }
    if (InitialTarget && (!IsValid(InitialTarget) || InitialTarget->GetWorld() != GetWorld() || InitialTarget->GetPlayerId() < 0))
    {
        return false;
    }
    FBBBMonsterDirectorTriggerEventWaveLocalControlPacket Packet;
    Packet.EventId = EventId;
    Packet.InitialTargetPlayerId = InitialTarget ? InitialTarget->GetPlayerId() : INDEX_NONE;
    return HasAuthority() && SubmitInput(MoveTemp(Packet));
}

bool ABBBMonsterDirectorPresentationActor::StopEventWave(const FName EventId)
{
    FBBBMonsterDirectorStopEventWaveLocalControlPacket Packet;
    Packet.EventId = EventId;
    return HasAuthority() && SubmitInput(MoveTemp(Packet));
}

void ABBBMonsterDirectorPresentationActor::ReadExcludeRegions(TArray<FBBBMonsterSpawnRegionDefinition>& Result) const
{
    Result.Reset();
    for (const ABBBMonsterSpawnRegionPresentationActor* Region : Regions)
    {
        if (IsValid(Region))
        {
            const auto Def = Region->ReadDefinition();
            if (Def.IsValid() && Def.Role == EBBBMonsterSpawnRegionRole::BlockSpawn)
            {
                Result.Add(Def);
            }
        }
    }
}

bool ABBBMonsterDirectorPresentationActor::ReadRegionsByIds(const TArray<FName>& RegionIds, TArray<FBBBMonsterSpawnRegionDefinition>& Result) const
{
    Result.Reset();
    for (const ABBBMonsterSpawnRegionPresentationActor* Region : Regions)
    {
        if (IsValid(Region))
        {
            const auto Def = Region->ReadDefinition();
            if (Def.IsValid() && Def.Role != EBBBMonsterSpawnRegionRole::BlockSpawn && RegionIds.Contains(Def.Id))
            {
                Result.Add(Def);
            }
        }
    }
    return !Result.IsEmpty();
}
