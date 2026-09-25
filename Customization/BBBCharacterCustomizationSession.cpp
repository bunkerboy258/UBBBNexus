#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "BBBWork/UBBBNexus/Client/BBBClientSubsystem.h"
#include "BBBWork/UBBBNexus/Customization/Appearance/BBBAppearanceComponent.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Containers/Ticker.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "PreviewScene.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomization, Log, All);

bool UBBBCharacterCustomizationSession::IsOpen() const
{
    return View != nullptr;
}

bool UBBBCharacterCustomizationSession::Open(APlayerController &Player)
{
    if (IsOpen() || !Player.IsLocalController() || !Player.GetPawn())
    {
        return false;
    }

    UBBBAppearanceComponent *Appearance = Player.GetPawn()->FindComponentByClass<UBBBAppearanceComponent>();
    if (!Appearance || PreviewActorClass.IsNull())
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("换装入口配置不完整 Appearance=%s PreviewClass=%s"),
            *GetNameSafe(Appearance), *PreviewActorClass.ToString());
        return false;
    }

    FBBBAppearanceSelection Selection = Appearance->GetAppearance();
    if (!Appearance->PrepareSelection(Selection) || !CreatePreview())
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("外观草稿或独立预览世界创建失败"));
        return false;
    }

    PreviewActor->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
    CurrentView = TEXT("Full");

    UBBBAppearanceComponent *PreviewAppearance = Preview.Get();
    if (!PreviewAppearance || !PreviewAppearance->PreviewAppearance(Selection))
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("预览人物无法组装当前外观"));
        return false;
    }

    UBBBCharacterCustomizationView *NewView = CreateWidget<UBBBCharacterCustomizationView>(
        &Player, UBBBCharacterCustomizationView::StaticClass());
    if (!NewView)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("换装界面创建失败"));
        return false;
    }

    Controller = &Player;
    Target = Appearance;
    Draft = MoveTemp(Selection);
    View = NewView;
    View->SetSession(this);
    View->SetPreviewTexture(PreviewTexture);
    View->AddToPlayerScreen();
    CaptureInput(Player);
    UpdateCamera();
    TimeSinceUpdate = 1.0f / 30.0f;

    UE_LOG(LogBBBCustomization, Log, TEXT("独立世界换装界面已打开 World=%s"),
        *GetNameSafe(Scene->GetWorld()));
    return true;
}

bool UBBBCharacterCustomizationSession::CreatePreview()
{
    if (Scene)
    {
        return PreviewActor != nullptr && CaptureActor != nullptr && Preview.IsValid();
    }

    UClass *PreviewClass = PreviewActorClass.LoadSynchronous();
    if (!PreviewClass)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("预览人物蓝图加载失败 Path=%s"), *PreviewActorClass.ToString());
        return false;
    }

    FPreviewScene::ConstructionValues Options;
    Options.SetEditor(false);
    Options.SetCreatePhysicsScene(false);
    Options.SetForceMipsResident(false);
    Options.SetLightRotation(FRotator(-35.0f, 0.0f, 0.0f));
    Options.SetLightBrightness(UE_PI * 2.0f);
    Scene = MakeUnique<FPreviewScene>(Options);
    UWorld *PreviewWorld = Scene->GetWorld();
    if (!PreviewWorld || PreviewWorld->GetNetDriver())
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("独立预览世界不可用或意外创建了网络驱动"));
        Shutdown();
        return false;
    }

    // 预览世界没有关卡环境光 反向补光保证旋转查看背面时仍可辨认
    UDirectionalLightComponent *FillLight = NewObject<UDirectionalLightComponent>(GetTransientPackage(), NAME_None, RF_Transient);
    FillLight->Intensity = UE_PI;
    FillLight->LightColor = FColor::White;
    Scene->AddComponent(FillLight, FTransform(FRotator(-30.0f, 180.0f, 0.0f)));

    PreviewActor = PreviewWorld->SpawnActor<AActor>(PreviewClass, FVector::ZeroVector, FRotator::ZeroRotator);
    CaptureActor = PreviewWorld->SpawnActor<ASceneCapture2D>();
    if (!PreviewActor || !CaptureActor)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("预览人物或捕获相机创建失败"));
        Shutdown();
        return false;
    }

    // 预览对象完全留在本机独立世界 不参与正式角色的复制
    PreviewActor->SetReplicates(false);
    UBBBAppearanceComponent *PreviewAppearance = PreviewActor->FindComponentByClass<UBBBAppearanceComponent>();
    if (!PreviewAppearance)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("预览人物缺少外观组装组件 Class=%s"),
            *GetNameSafe(PreviewClass));
        Shutdown();
        return false;
    }
    PreviewAppearance->SetIsReplicated(false);
    Preview = PreviewAppearance;

    PreviewTexture = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Transient);
    PreviewTexture->RenderTargetFormat = RTF_RGBA8;
    PreviewTexture->InitAutoFormat(1024, 1024);
    PreviewTexture->UpdateResourceImmediate(true);

    USceneCaptureComponent2D *CaptureComponent = CaptureActor->GetCaptureComponent2D();
    CaptureComponent->TextureTarget = PreviewTexture;
    CaptureComponent->bCaptureEveryFrame = false;
    CaptureComponent->bCaptureOnMovement = false;
    CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    CaptureComponent->FOVAngle = 35.0f;

    // FPreviewScene 初始化世界但不会代替游戏主循环分发 BeginPlay 和 Tick
    if (!PreviewWorld->GetBegunPlay())
    {
        for (TActorIterator<AActor> Actor(PreviewWorld); Actor; ++Actor)
        {
            Actor->DispatchBeginPlay();
        }
        PreviewWorld->SetBegunPlay(true);
    }

    TickHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &ThisClass::TickPreview));
    return true;
}

bool UBBBCharacterCustomizationSession::TickPreview(const float DeltaTime)
{
    if (!Scene)
    {
        return false;
    }

    TimeSinceUpdate += FMath::Min(DeltaTime, 0.2f);
    const float Interval = IsOpen() ? 1.0f / 30.0f : 0.2f;
    if (TimeSinceUpdate < Interval)
    {
        return true;
    }

    const float Step = TimeSinceUpdate;
    TimeSinceUpdate = 0.0f;
    Scene->GetWorld()->Tick(LEVELTICK_All, Step);

    if (IsOpen())
    {
        Capture();
    }
    return true;
}

void UBBBCharacterCustomizationSession::Capture()
{
    if (!CaptureActor || !PreviewTexture)
    {
        return;
    }

    CaptureActor->GetCaptureComponent2D()->CaptureScene();
}

void UBBBCharacterCustomizationSession::UpdateCamera()
{
    if (!PreviewActor || !CaptureActor)
    {
        return;
    }

    FVector Center;
    FVector Extent;
    PreviewActor->GetActorBounds(false, Center, Extent);
    if (Extent.IsNearlyZero())
    {
        UE_LOG(LogBBBCustomization, Warning, TEXT("预览人物无有效网格边界"));
        return;
    }

    FVector Focus = Center;
    float Distance = FMath::Max(Extent.Z * 4.3f, 360.0f);
    if (CurrentView == TEXT("Head"))
    {
        Focus.Z += Extent.Z * 0.65f;
        Distance = FMath::Max(Extent.Z * 2.0f, 200.0f);
    }
    if (CurrentView == TEXT("Legs"))
    {
        Focus.Z -= Extent.Z * 0.55f;
        Distance = FMath::Max(Extent.Z * 2.2f, 220.0f);
    }

    const FVector CameraLocation = Focus - FVector(Distance, 0.0f, 0.0f);
    CaptureActor->SetActorLocationAndRotation(CameraLocation, (Focus - CameraLocation).Rotation());
}

void UBBBCharacterCustomizationSession::CaptureInput(APlayerController &Player)
{
    bPreviousCursor = Player.bShowMouseCursor;
    if (UBBBPlayerInputSystem *Input = Player.FindComponentByClass<UBBBPlayerInputSystem>())
    {
        bPreviousGameplayInput = Input->IsInputEnabled();
        Input->SetInputEnabled(false);
    }

    bInputCaptured = true;
    Player.SetIgnoreMoveInput(true);
    Player.SetIgnoreLookInput(true);
    Player.bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(View->TakeWidget());
    Player.SetInputMode(InputMode);
}

void UBBBCharacterCustomizationSession::RestoreInput()
{
    APlayerController *Player = Controller.Get();
    if (!Player || !bInputCaptured)
    {
        return;
    }

    // 只撤销本会话增加的输入忽略计数 不干扰其它界面的输入锁
    Player->SetIgnoreMoveInput(false);
    Player->SetIgnoreLookInput(false);
    Player->bShowMouseCursor = bPreviousCursor;
    if (UBBBPlayerInputSystem *Input = Player->FindComponentByClass<UBBBPlayerInputSystem>())
    {
        Input->SetInputEnabled(bPreviousGameplayInput);
    }
    if (bPreviousCursor)
    {
        Player->SetInputMode(FInputModeGameAndUI());
    }
    if (!bPreviousCursor)
    {
        Player->SetInputMode(FInputModeGameOnly());
    }
}

void UBBBCharacterCustomizationSession::Close()
{
    if (View)
    {
        View->RemoveFromParent();
        View = nullptr;
    }

    RestoreInput();
    bInputCaptured = false;
    Controller.Reset();
    Target.Reset();
    Draft = {};
}

void UBBBCharacterCustomizationSession::Shutdown()
{
    Close();
    if (TickHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
        TickHandle.Reset();
    }

    if (Scene && Scene->GetWorld())
    {
        Scene->GetWorld()->EndPlay(EEndPlayReason::RemovedFromWorld);
    }
    Preview.Reset();
    PreviewActor = nullptr;
    CaptureActor = nullptr;
    PreviewTexture = nullptr;
    Scene.Reset();
    TimeSinceUpdate = 0.0f;
}

void UBBBCharacterCustomizationSession::BeginDestroy()
{
    Shutdown();
    Super::BeginDestroy();
}

FBBBAppearanceSelection UBBBCharacterCustomizationSession::GetDraft() const
{
    return Draft;
}

bool UBBBCharacterCustomizationSession::SetDraft(FBBBAppearanceSelection Selection)
{
    if (!IsOpen() || !Target.IsValid() || !Preview.IsValid()
        || !Target->PrepareSelection(Selection) || !Preview->PreviewAppearance(Selection))
    {
        UE_LOG(LogBBBCustomization, Warning, TEXT("草稿检查或独立预览组装失败"));
        return false;
    }

    Draft = MoveTemp(Selection);
    UpdateCamera();
    return true;
}

bool UBBBCharacterCustomizationSession::CycleItem(const FName Slot, const int32 Direction)
{
    if (!IsOpen() || !Target.IsValid() || Direction == 0)
    {
        return false;
    }

    const TArray<FName> Items = Target->GetItems(Slot);
    if (Items.IsEmpty())
    {
        return false;
    }

    FBBBAppearanceSelection Selection = Draft;
    FName *Selected = nullptr;
    for (FBBBAppearancePart &Part : Selection.Parts)
    {
        if (Part.Slot == Slot)
        {
            Selected = &Part.Item;
            break;
        }
    }
    if (Slot == TEXT("Attachments"))
    {
        Selected = &Selection.Attachments;
    }
    if (!Selected)
    {
        return false;
    }

    const int32 Index = Items.IndexOfByKey(*Selected);
    const int32 Step = Direction > 0 ? 1 : -1;
    const int32 Start = Index == INDEX_NONE ? (Step > 0 ? -1 : 0) : Index;
    *Selected = Items[(Start + Step + Items.Num()) % Items.Num()];
    return SetDraft(MoveTemp(Selection));
}

bool UBBBCharacterCustomizationSession::SetSurface(const float Dirt, const float Weathering)
{
    FBBBAppearanceSelection Selection = Draft;
    Selection.Dirt = Dirt;
    Selection.Weathering = Weathering;
    return SetDraft(MoveTemp(Selection));
}

bool UBBBCharacterCustomizationSession::Apply()
{
    APlayerController *Player = Controller.Get();
    if (!IsOpen() || !Player || !Target.IsValid()
        || Target->GetOwner() != Player->GetPawn() || !Target->CommitAppearance(Draft))
    {
        UE_LOG(LogBBBCustomization, Warning, TEXT("应用失败 本地目标已经变化或外观组装失败"));
        return false;
    }

    ULocalPlayer *LocalPlayer = Player->GetLocalPlayer();
    UBBBClientSubsystem *Client = LocalPlayer ? LocalPlayer->GetSubsystem<UBBBClientSubsystem>() : nullptr;
    if (!Client || !Client->SaveAppearance(Draft))
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("外观已应用 但本地保存失败"));
        return false;
    }
    UE_LOG(LogBBBCustomization, Log, TEXT("外观最终组合已应用并保存"));
    return true;
}

void UBBBCharacterCustomizationSession::SelectView(const FName ViewName)
{
    if (ViewName != TEXT("Full") && ViewName != TEXT("Head") && ViewName != TEXT("Legs"))
    {
        UE_LOG(LogBBBCustomization, Warning, TEXT("未知预览机位 View=%s"), *ViewName.ToString());
        return;
    }

    CurrentView = ViewName;
    UpdateCamera();
}

void UBBBCharacterCustomizationSession::RotatePreview(const float Degrees)
{
    if (!IsOpen() || !PreviewActor || !FMath::IsFinite(Degrees))
    {
        return;
    }

    PreviewActor->AddActorWorldRotation(FRotator(0.0f, Degrees, 0.0f));
}
