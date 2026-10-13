#include "BBBMonsterDirectorPresentationActor.h"
#include "BBBMonsterSpawnRegionPresentationActor.h"
#include "Components/SceneComponent.h"
#include "MassEntityConfigAsset.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterDirectorTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterDirectorNetworkComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Director/BBBMonsterDirectorParseProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Director/BBBMonsterDirectorPlacementProcessor.h"

ABBBMonsterDirectorPresentationActor::ABBBMonsterDirectorPresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;
    SetNetUpdateFrequency(5.0f);
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DirectorRoot"));
    DirectorNetwork = CreateDefaultSubobject<UBBBMonsterDirectorNetworkComponent>(TEXT("DirectorNetwork"));
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
        Seen.Add(Config.Id);
    }
    for (const auto& Stage : Definition->Stages)
    {
        for (const FName Id : Stage.RegionIds)
        {
            if (!Seen.Contains(Id))
            {
                Error = FString::Printf(TEXT("阶段 %s 缺少地图区域 %s"), *Stage.Id.ToString(), *Id.ToString());
                return false;
            }
        }
    }
    Error.Reset();
    return true;
}

bool ABBBMonsterDirectorPresentationActor::EnsureEntity()
{
    if (!GetWorld() || !GetWorld()->IsGameWorld() || !EntityConfig)
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
    if (HasAuthority())
    {
        FString Error;
        if (!ValidateConfiguration(Error))
        {
            UE_LOG(LogTemp, Error, TEXT("[BBBMonsterDirector] %s %s"), *GetName(), *Error);
            return false;
        }
    }
    Entity = Mass->CreateEntity(*EntityConfig);
    if (!Entity.IsSet())
    {
        return false;
    }
    if (HasAuthority())
    {
        FBBBMonsterDirectorInitializeLocalControlPacket Packet;
        Packet.InstanceId = FGuid::NewGuid();
        Packet.Definition = Definition.Get();
        Packet.Presentation = this;
        for (const ABBBMonsterSpawnRegionPresentationActor* Region : Regions)
        {
            Packet.Regions.Add(Region->ReadDefinition());
        }
        Mass->SubmitInput(Entity, MoveTemp(Packet));
    }
    return true;
}

void ABBBMonsterDirectorPresentationActor::BeginPlay()
{
    Super::BeginPlay();
    if (EnsureEntity() && HasAuthority() && bAutoStart)
    {
        StartDirector();
    }
}

void ABBBMonsterDirectorPresentationActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (auto* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>())
    {
        Mass->DestroyEntity(Entity);
    }
    Entity.Reset();
    Super::EndPlay(EndPlayReason);
}

FBBBMonsterDirectorReplicationState ABBBMonsterDirectorPresentationActor::ReadState() const
{
    FBBBMonsterDirectorReplicationState Result;
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

bool ABBBMonsterDirectorPresentationActor::PauseDirector()
{
    return HasAuthority() && SubmitInput(FBBBMonsterDirectorPauseLocalControlPacket());
}

bool ABBBMonsterDirectorPresentationActor::ResumeDirector()
{
    return HasAuthority() && SubmitInput(FBBBMonsterDirectorResumeLocalControlPacket());
}

bool ABBBMonsterDirectorPresentationActor::StopDirector()
{
    return HasAuthority() && SubmitInput(FBBBMonsterDirectorStopLocalControlPacket());
}

bool ABBBMonsterDirectorPresentationActor::TriggerStage()
{
    const auto State = ReadState();
    FBBBMonsterDirectorTriggerLocalControlPacket Packet;
    Packet.Generation = State.Generation;
    Packet.StageIndex = State.StageIndex;
    return HasAuthority() && SubmitInput(MoveTemp(Packet));
}

bool ABBBMonsterDirectorPresentationActor::ReadPlacementRegions(const int32 StageIndex, TArray<FBBBMonsterSpawnRegionDefinition>& Result) const
{
    Result.Reset();
    if (!GetWorld() || !Definition || !Definition->Stages.IsValidIndex(StageIndex))
    {
        return false;
    }
    if (GetWorld()->IsGameWorld())
    {
        return UBBBMonsterDirectorPlacementProcessor::QueryRegions(*GetWorld(), Entity, StageIndex, Result);
    }
    for (const ABBBMonsterSpawnRegionPresentationActor* Region : Regions)
    {
        if (IsValid(Region) && Definition->Stages[StageIndex].RegionIds.Contains(Region->ReadDefinition().Id))
        {
            Result.Add(Region->ReadDefinition());
        }
    }
    return !Result.IsEmpty();
}

UBBBMonsterDirectorNetworkComponent* ABBBMonsterDirectorPresentationActor::GetDirectorNetwork() const
{
    return DirectorNetwork;
}
