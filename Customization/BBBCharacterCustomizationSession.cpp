#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "BBBWork/UBBBNexus/Client/BBBClientSubsystem.h"
#include "BBBWork/UBBBNexus/Customization/Appearance/BBBAppearanceComponent.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/RectLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Containers/Ticker.h"
#include "ContentStreaming.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureCube.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
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
    Options.SetCreateDefaultLighting(false);
    Scene = MakeUnique<FPreviewScene>(Options);
    UWorld *PreviewWorld = Scene->GetWorld();
    if (!PreviewWorld || PreviewWorld->GetNetDriver())
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("独立预览世界不可用或意外创建了网络驱动"));
        Shutdown();
        return false;
    }

    // 预览世界没有关卡环境光 反向补光保证旋转查看背面时仍可辨认
    if (!ConfigurePreviewLighting())
    {
        Shutdown();
        return false;
    }

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
    CaptureComponent->bAlwaysPersistRenderingState = true;
    CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    CaptureComponent->FOVAngle = 35.0f;

    // 固定曝光避免切换衣服与机位时亮度漂移 保留暗部而不依赖强泛光
    FPostProcessSettings &PostProcess = CaptureComponent->PostProcessSettings;
    PostProcess.bOverride_AutoExposureMethod = true;
    PostProcess.AutoExposureMethod = AEM_Manual;
    PostProcess.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
    PostProcess.AutoExposureApplyPhysicalCameraExposure = false;
    PostProcess.bOverride_AutoExposureBias = true;
    PostProcess.AutoExposureBias = 0.0f;
    PostProcess.bOverride_BloomIntensity = true;
    PostProcess.BloomIntensity = 0.0f;
    PostProcess.bOverride_VignetteIntensity = true;
    PostProcess.VignetteIntensity = 0.15f;
    CaptureComponent->PostProcessBlendWeight = 1.0f;

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

bool UBBBCharacterCustomizationSession::ConfigurePreviewLighting()
{
    UMaterialInterface *BackdropMaterial = PreviewBackdropMaterial.LoadSynchronous();
    UStaticMesh *BackdropMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    UTextureCube *AmbientCubemap = LoadObject<UTextureCube>(nullptr, TEXT("/Engine/EngineMaterials/DefaultCubemap.DefaultCubemap"));
    if (!BackdropMaterial || !BackdropMesh || !AmbientCubemap)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("预览灯光资源不完整 Backdrop=%s Mesh=%s Cubemap=%s"),
            *PreviewBackdropMaterial.ToString(), *GetNameSafe(BackdropMesh), *GetNameSafe(AmbientCubemap));
        return false;
    }

    // 灯光留在预览世界而不挂到人物上 旋转人物时照明方向保持不变
    const FVector Focus(0.0f, 0.0f, 120.0f);
    URectLightComponent *KeyLight = NewObject<URectLightComponent>(this, TEXT("KeyLight"), RF_Transient);
    KeyLight->SetIntensityUnits(ELightUnits::Lumens);
    KeyLight->SetIntensity(120.0f);
    KeyLight->SetLightColor(FLinearColor(FColor(242, 247, 255)));
    KeyLight->SetSourceWidth(60.0f);
    KeyLight->SetSourceHeight(80.0f);
    KeyLight->SetAttenuationRadius(1200.0f);
    const FVector KeyPosition(-260.0f, -220.0f, 260.0f);
    Scene->AddComponent(KeyLight, FTransform((Focus - KeyPosition).Rotation(), KeyPosition));

    URectLightComponent *FillLight = NewObject<URectLightComponent>(this, TEXT("FillLight"), RF_Transient);
    FillLight->SetIntensityUnits(ELightUnits::Lumens);
    FillLight->SetIntensity(60.0f);
    FillLight->SetLightColor(FLinearColor(0.65f, 0.76f, 1.0f));
    FillLight->SetSourceWidth(220.0f);
    FillLight->SetSourceHeight(260.0f);
    FillLight->SetAttenuationRadius(1200.0f);
    FillLight->SetCastShadows(false);
    const FVector FillPosition(-200.0f, 240.0f, 180.0f);
    Scene->AddComponent(FillLight, FTransform((Focus - FillPosition).Rotation(), FillPosition));

    URectLightComponent *RimLight = NewObject<URectLightComponent>(this, TEXT("RimLight"), RF_Transient);
    RimLight->SetIntensityUnits(ELightUnits::Lumens);
    RimLight->SetIntensity(150.0f);
    RimLight->SetLightColor(FLinearColor(0.48f, 0.38f, 1.0f));
    RimLight->SetSourceWidth(100.0f);
    RimLight->SetSourceHeight(220.0f);
    RimLight->SetAttenuationRadius(1200.0f);
    RimLight->SetCastShadows(false);
    const FVector RimPosition(200.0f, 180.0f, 250.0f);
    Scene->AddComponent(RimLight, FTransform((Focus - RimPosition).Rotation(), RimPosition));

    // 少量环境光与反射托住眼窝和背面 主光仍决定形体 不实时捕获背景
    USkyLightComponent *AmbientLight = NewObject<USkyLightComponent>(this, TEXT("AmbientLight"), RF_Transient);
    AmbientLight->SetMobility(EComponentMobility::Movable);
    AmbientLight->SourceType = SLS_SpecifiedCubemap;
    AmbientLight->SetCubemap(AmbientCubemap);
    AmbientLight->SetIntensity(0.1f);
    AmbientLight->bLowerHemisphereIsBlack = false;
    Scene->AddComponent(AmbientLight, FTransform::Identity);

    // 背景不是人物部件 不计入人物包围盒 也不影响预览机位的自动取景
    UStaticMeshComponent *Backdrop = NewObject<UStaticMeshComponent>(this, TEXT("PreviewBackdrop"), RF_Transient);
    Backdrop->SetStaticMesh(BackdropMesh);
    Backdrop->SetMaterial(0, BackdropMaterial);
    Backdrop->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Backdrop->SetCastShadow(false);
    Scene->AddComponent(Backdrop, FTransform(FRotator::ZeroRotator, Focus, FVector(30.0f)));
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

    USceneCaptureComponent2D *CaptureComponent = CaptureActor->GetCaptureComponent2D();
    // 独立世界没有玩家视口 用捕获相机提交纹理需求 关闭预览后不再维持高分辨率请求
    const float ScreenSize = static_cast<float>(PreviewTexture->SizeX);
    const float FOVScreenSize = ScreenSize / FMath::Tan(FMath::DegreesToRadians(CaptureComponent->FOVAngle * 0.5f));
    IStreamingManager::Get().AddViewInformation(CaptureActor->GetActorLocation(), ScreenSize,
        FOVScreenSize, 1.0f, false, 0.0f, PreviewActor.Get(), Scene->GetWorld());
    CaptureComponent->CaptureScene();
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

bool UBBBCharacterCustomizationSession::CyclePatch(const FName Slot, const int32 Direction)
{
    if (!IsOpen() || Direction == 0 || (Slot != TEXT("Body") && Slot != TEXT("Vest")))
    {
        return false;
    }

    FBBBAppearanceSelection Selection = Draft;
    for (FBBBAppearancePart &Part : Selection.Parts)
    {
        if (Part.Slot != Slot)
        {
            continue;
        }

        // 作者图集按八行八列排列 草稿直接保存材质使用的坐标
        const int32 Column = FMath::RoundToInt(Part.Patch.X * 8.0);
        const int32 Row = FMath::RoundToInt(Part.Patch.Y * 8.0);
        const int32 Step = Direction > 0 ? 1 : -1;
        const int32 Index = (Row * 8 + Column + Step + 64) % 64;
        Part.Patch = FVector2D(Index % 8, Index / 8) / 8.0;
        return SetDraft(MoveTemp(Selection));
    }

    UE_LOG(LogBBBCustomization, Warning, TEXT("徽章部位不存在 Slot=%s"), *Slot.ToString());
    return false;
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
