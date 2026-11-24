#include "BBBWork/UBBBNexus/Player/BBBPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundWave.h"
#include "UObject/ConstructorHelpers.h"


//创建并配置ABBB玩家状态
ABBBPlayerState::ABBBPlayerState()
{
    //设置NetUpdateFrequency
    SetNetUpdateFrequency(100.0f);
    //设置MinNetUpdateFrequency
    SetMinNetUpdateFrequency(30.0f);

    static ConstructorHelpers::FObjectFinder<USoundWave> Confirmation(
        TEXT("/Game/_ThirdParty/Audio/SFXMint/SFXMint_Elimination_Hitmarker_04.SFXMint_Elimination_Hitmarker_04"));
    KillConfirmationSound = Confirmation.Object;
}

//进入游戏时建立运行依赖
void ABBBPlayerState::BeginPlay()
{
    //先执行父类的进入游戏时建立运行依赖
    Super::BeginPlay();
}

void ABBBPlayerState::PostNetInit()
{
    Super::PostNetInit();
    bKillCountsInitialized = true;
}

void ABBBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ABBBPlayerState, KillCounts, COND_OwnerOnly);
    DOREPLIFETIME(ABBBPlayerState, FactionName);
}

void ABBBPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);
    if (auto* Target = Cast<ABBBPlayerState>(PlayerState))
    {
        Target->FactionName = FactionName;
    }
}

void ABBBPlayerState::SetFactionName(const FName InFactionName)
{
    if (!HasAuthority() || InFactionName.IsNone())
    {
        return;
    }
    FactionName = InFactionName;
    ForceNetUpdate();
}

void ABBBPlayerState::AddConfirmedKills(const FBBBPlayerKillCounts& ConfirmedKills)
{
    if (!HasAuthority() || (ConfirmedKills.CommonKills <= 0 && ConfirmedKills.SpecialKills <= 0))
    {
        return;
    }

    KillCounts.CommonKills += ConfirmedKills.CommonKills;
    KillCounts.SpecialKills += ConfirmedKills.SpecialKills;
    ForceNetUpdate();
    PlayKillConfirmation();
}

void ABBBPlayerState::OnRep_KillCounts(const FBBBPlayerKillCounts& Previous)
{
    if (bKillCountsInitialized &&
        (KillCounts.CommonKills > Previous.CommonKills || KillCounts.SpecialKills > Previous.SpecialKills))
    {
        PlayKillConfirmation();
    }
}

void ABBBPlayerState::PlayKillConfirmation() const
{
    const APlayerController* Controller = GetPlayerController();
    if (KillConfirmationSound && Controller && Controller->IsLocalPlayerController())
    {
        UGameplayStatics::PlaySound2D(this, KillConfirmationSound);
    }
}
