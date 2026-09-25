#include "BBBWork/UBBBNexus/Client/BBBClientSubsystem.h"
#include "BBBWork/UBBBNexus/Client/Save/BBBClientSaveGame.h"
#include "BBBWork/UBBBNexus/Customization/Appearance/BBBAppearanceComponent.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBClient, Log, All);

void UBBBClientSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);
    const int32 User = FMath::Max(GetLocalPlayer()->GetControllerId(), 0);
    if (UGameplayStatics::DoesSaveGameExist(TEXT("BBBClient"), User))
    {
        Settings = Cast<UBBBClientSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("BBBClient"), User));
        if (!Settings)
        {
            UE_LOG(LogBBBClient, Error, TEXT("本地配置存在但无法读取 User=%d"), User);
        }
    }
    if (!Settings)
    {
        Settings = NewObject<UBBBClientSaveGame>(this);
    }
}

void UBBBClientSubsystem::Deinitialize()
{
    if (Customization)
    {
        Customization->Shutdown();
        Customization = nullptr;
    }
    if (Controller.IsValid())
    {
        Controller->OnPossessedPawnChanged.RemoveAll(this);
    }
    Super::Deinitialize();
}

void UBBBClientSubsystem::PlayerControllerChanged(APlayerController *NewPlayerController)
{
    if (Customization)
    {
        Customization->Close();
    }
    if (Controller.IsValid())
    {
        Controller->OnPossessedPawnChanged.RemoveAll(this);
    }
    Controller = NewPlayerController;
    if (NewPlayerController)
    {
        NewPlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::OnPawnChanged);
        OnPawnChanged(nullptr, NewPlayerController->GetPawn());
    }
}

void UBBBClientSubsystem::OnPawnChanged(APawn *OldPawn, APawn *NewPawn)
{
    if (Customization)
    {
        Customization->Close();
    }
    if (!NewPawn || !Settings || !Settings->Appearance.IsValid())
    {
        return;
    }

    //控制权通知可能早于组件 BeginPlay 下一帧恢复并重新核对拥有者 避免写入旧人物
    const TWeakObjectPtr<APawn> Pawn = NewPawn;
    NewPawn->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, Pawn]()
    {
        if (!Pawn.IsValid() || !Controller.IsValid() || Controller->GetPawn() != Pawn.Get())
        {
            return;
        }
        UBBBAppearanceComponent *Appearance = Pawn->FindComponentByClass<UBBBAppearanceComponent>();
        if (!Appearance)
        {
            return;
        }
        FBBBAppearanceSelection Selection = Settings->Appearance;
        if (!Appearance->PrepareSelection(Selection) || !Appearance->CommitAppearance(Selection))
        {
            UE_LOG(LogBBBClient, Warning, TEXT("本地外观恢复失败 保留蓝图默认配置"));
        }
    }));
}

bool UBBBClientSubsystem::SaveAppearance(const FBBBAppearanceSelection &Selection)
{
    if (!Settings || !Selection.IsValid())
    {
        return false;
    }
    Settings->Appearance = Selection;
    const int32 User = FMath::Max(GetLocalPlayer()->GetControllerId(), 0);
    const bool bSaved = UGameplayStatics::SaveGameToSlot(Settings, TEXT("BBBClient"), User);
    if (!bSaved)
    {
        UE_LOG(LogBBBClient, Error, TEXT("本地玩家配置写入失败 User=%d"), User);
    }
    return bSaved;
}

void UBBBClientSubsystem::ToggleCustomization()
{
    APlayerController *Player = GetLocalPlayer()->GetPlayerController(GetWorld());
    if (!Player)
    {
        return;
    }
    if (!Customization)
    {
        Customization = NewObject<UBBBCharacterCustomizationSession>(this);
    }
    if (Customization->IsOpen())
    {
        Customization->Close();
        return;
    }
    Customization->Open(*Player);
}
