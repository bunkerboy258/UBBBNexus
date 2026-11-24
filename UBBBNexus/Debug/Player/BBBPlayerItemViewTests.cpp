#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudWidget.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "MassCommonFragments.h"
#include "Components/BoxComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Input/HittestGrid.h"
#include "Layout/WidgetPath.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/SceneCapture2D.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Containers/Ticker.h"
#include "UnrealClient.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "HAL/IConsoleManager.h"
#include "DynamicRHI.h"
#include "RenderTimer.h"
#include "Frontend/BBBFrontendSubsystem.h"
#include "Misc/ScopeExit.h"
#include "Widgets/Input/SButton.h"

namespace
{
void SendReviewPointer(const UBBBPlayerItemView& View, FVector2D From, FVector2D To, int32 Phase = 0)
{
    const auto Window = GEngine->GameViewport ? GEngine->GameViewport->GetWindow() : nullptr;
    if (!Window)
    {
        return;
    }
    const FGeometry& Geometry = View.GetCachedGeometry();
    const auto Project = [&Geometry](FVector2D Point)
    {
        return Geometry.LocalToAbsolute(Point / FVector2D(1920.0, 1080.0) * Geometry.GetLocalSize());
    };
    auto& Application = FSlateApplication::Get();
    const FVector2D Start = Project(From);
    const FVector2D End = Project(To);
    const auto HitPath = [&Window](FVector2D Desktop)
    {
        auto Path = Window->GetHittestGrid().GetBubblePath(Desktop, 0, false, 0);
        return FWidgetPath(MakeArrayView(Path));
    };
    const FKey Left = EKeys::LeftMouseButton;
    const TSet<FKey> Pressed{Left};
    const auto Event = [](FVector2D Position, FVector2D Previous, const TSet<FKey>& Buttons, FKey Changed)
    {
        return FPointerEvent(FSlateApplication::CursorPointerIndex, Position, Previous, Buttons, Changed, 0, FModifierKeysState());
    };
    const FVector2D Near = Project(From + FVector2D(25, 0));
    const FVector2D Middle = (Start + End) * 0.5;
    if (Phase == 0)
    {
        Application.ReleaseAllPointerCapture();
        Application.SetCursorPos(Start);
        const auto StartPath = HitPath(Start);
        if (!StartPath.IsValid())
        {
            return;
        }
        Application.RoutePointerMoveEvent(StartPath, Event(Start, Start - FVector2D(2, 2), {}, FKey()), false);
        Application.RoutePointerDownEvent(StartPath, Event(Start, Start, Pressed, Left));
        if (From == To)
        {
            Application.RoutePointerUpEvent(StartPath, Event(Start, Start, {}, Left));
        }
        return;
    }
    if (Phase == 1)
    {
        Application.SetCursorPos(Near);
        Application.RoutePointerMoveEvent(HitPath(Near), Event(Near, Start, Pressed, FKey()), false);
        Application.SetCursorPos(Middle);
        Application.RoutePointerMoveEvent(HitPath(Middle), Event(Middle, Near, Pressed, FKey()), false);
        return;
    }
    if (Phase == 2)
    {
        Application.SetCursorPos(End);
        Application.RoutePointerMoveEvent(HitPath(End), Event(End, Middle, Pressed, FKey()), false);
        return;
    }
    Application.RoutePointerUpEvent(HitPath(End), Event(End, End, {}, Left));
}

}

class FBBBItemRoomReview final
{
public:
    FBBBItemRoomReview()
    {
        bLive = FParse::Value(FCommandLine::Get(), TEXT("ItemMenuReviewDir="), Directory);
        if (!bLive)
        {
            Directory = FPaths::ScreenShotDir();
        }
        ReadyDeadline = FPlatformTime::Seconds() + 150.0;
    }

    bool Tick(float)
    {
        if (!View.IsValid())
        {
            FindView();
            return View.IsValid() || FPlatformTime::Seconds() < ReadyDeadline;
        }

        UBBBPlayerItemView& ItemView = *View.Get();
        UBBBPlayerItemPortrait* Portrait = ItemView.CharacterPreview.Get();
        if (!bPerfComplete && FParse::Param(FCommandLine::Get(), TEXT("ItemMenuPerf")))
        {
            bPerfComplete = !TickPerformance(ItemView, Portrait);
            return true;
        }
        if (!Portrait || !Portrait->Stage || !Portrait->Stage->GetKey() || Portrait->GetPanels().Num() != 3)
        {
            return FPlatformTime::Seconds() < ReadyDeadline;
        }

        if (const auto* Frontend = ItemView.GetWorld()->GetGameInstance()->GetSubsystem<UBBBFrontendSubsystem>())
        {
            const auto& Presentation = Frontend->RuntimeData.Frontend.ReadBBBFrontendState();
            if (Presentation.bLoading)
            {
                return FPlatformTime::Seconds() < ReadyDeadline;
            }
        }

        if (bReadingCapture)
        {
            return AwaitCapture(*Portrait);
        }
        if (FScreenshotRequest::IsScreenshotRequested())
        {
            return true;
        }
        Portrait->ReviewPoseTime = -1.0f;

        FString Request;
        if (bLive)
        {
            if (!FFileHelper::LoadFileToString(Request, *(Directory / TEXT("request.txt"))))
            {
                return true;
            }
        }
        else
        {
            static const TCHAR* Requests[] = {
                TEXT("Name=ItemMenu_Room Page=Room"),
                TEXT("Name=ItemMenu_Inventory Page=Inventory Drag=1"),
                TEXT("Name=ItemMenu_Character Page=Character")};
            if (AutomaticIndex == UE_ARRAY_COUNT(Requests))
            {
                return false;
            }
            Request = Requests[AutomaticIndex];
        }

        FString Name;
        if (!FParse::Value(*Request, TEXT("Name="), Name) || Name == LastName ||
            Name.IsEmpty() || Name != FPaths::MakeValidFileName(Name) || Name == TEXT(".") || Name == TEXT(".."))
        {
            return true;
        }
        LastName = MoveTemp(Name);
        Configure(ItemView, *Portrait, *Request);
        return true;
    }

private:
    void FindView()
    {
        if (!GEngine || !GEngine->GameViewport)
        {
            return;
        }

        UWorld* World = GEngine->GameViewport->GetWorld();
        if (!World || World->GetNetMode() != NM_Standalone || !World->GetMapName().EndsWith(TEXT("BBBTest")))
        {
            return;
        }

        auto* Controller = Cast<ABBBPlayerController>(World->GetFirstPlayerController());
        if (!Controller || !Controller->HasItemInventory())
        {
            return;
        }

        if (!bSeeded)
        {
            const TCHAR* Samples[] = {TEXT("ModernWeapons_Rifle_01"), TEXT("ModernWeapons_Pistol_01"),
                TEXT("ModernWeapons_Shotgun_02"), TEXT("ModernWeapons_Rifle_04"), TEXT("ModernWeapons_LMG_01"),
                TEXT("ModernWeapons_Rifle_02"), TEXT("ModernWeapons_Shotgun_03"), TEXT("Grenade_01")};
            for (const TCHAR* Id : Samples)
            {
                Controller->SubmitItemAdd(FName(Id));
            }
            bSeeded = true;
            return;
        }

        if (!bEquipped)
        {
            int32 Start = INDEX_NONE;
            int32 Count = 0;
            Controller->GetStorageRange(EBBBItemType::Wearable, Start, Count);
            for (int32 Slot = Start; Slot < Start + FMath::Min(2, Count); ++Slot)
            {
                const auto Item = Controller->GetItemDisplayData(Slot);
                if (Item.bOccupied)
                {
                    Controller->SubmitItemMove(Slot, Item.EquipSlot, Item.InstanceId);
                }
            }
            bEquipped = true;
            return;
        }

        if (!Controller->IsBackpackOpen())
        {
            if (FParse::Param(FCommandLine::Get(), TEXT("ItemMenuPerf")))
            {
                const auto* Frontend = World->GetGameInstance()->GetSubsystem<UBBBFrontendSubsystem>();
                if (Frontend && Frontend->RuntimeData.Frontend.ReadBBBFrontendState().bLoading)
                {
                    return;
                }
            }
            PerfOpenAt = FPlatformTime::Seconds();
            Controller->ToggleBackpack();
            PerfToggleMs = (FPlatformTime::Seconds() - PerfOpenAt) * 1000.0;
        }
        TArray<UUserWidget*> Views;
        UWidgetBlueprintLibrary::GetAllWidgetsOfClass(Controller, Views, UBBBPlayerItemView::StaticClass(), false);
        for (UUserWidget* Candidate : Views)
        {
            auto* ItemView = CastChecked<UBBBPlayerItemView>(Candidate);
            if (ItemView->IsBackpackOpen())
            {
                View = ItemView;
                break;
            }
        }
    }

    bool TickPerformance(UBBBPlayerItemView& ItemView, const UBBBPlayerItemPortrait* Portrait)
    {
        const double Now = FPlatformTime::Seconds();
        const double FrameMs = (Now - PerfPreviousFrame) * 1000.0;
        PerfPreviousFrame = Now;
        if (Now >= ReadyDeadline)
        {
            UE_LOG(LogTemp, Error, TEXT("ItemRoomPerf timed out"));
            return false;
        }

        if (PerfOpenCount < 6)
        {
            auto* Controller = ItemView.GetItemController();
            if (!ItemView.IsBackpackOpen())
            {
                if (++PerfFrame == 12)
                {
                    PerfBeforeCapture = Portrait ? Portrait->CaptureRevision : 0;
                    PerfOpenAt = FPlatformTime::Seconds();
                    Controller->ToggleBackpack();
                    PerfToggleMs = (FPlatformTime::Seconds() - PerfOpenAt) * 1000.0;
                    PerfFrame = 0;
                }
                return true;
            }
            if (!Portrait || Portrait->CaptureRevision == PerfBeforeCapture || !Portrait->GetTexture())
            {
                return true;
            }
            if (PerfFrame == 0)
            {
                UE_LOG(LogTemp, Display, TEXT("ItemRoomPerf Open=%d ToggleMs=%.3f CaptureMs=%.3f"),
                    PerfOpenCount, PerfToggleMs, (Now - PerfOpenAt) * 1000.0);
            }
            if (++PerfFrame == 18)
            {
                ++PerfOpenCount;
                PerfFrame = 0;
                if (PerfOpenCount < 6)
                {
                    Controller->ToggleBackpack();
                }
            }
            return true;
        }

        if (PerfFrame >= 8)
        {
            PerfWallMs.Add(FrameMs);
            PerfGameMs.Add(FPlatformTime::ToMilliseconds(GGameThreadTime));
            PerfRenderMs.Add(FPlatformTime::ToMilliseconds(GRenderThreadTime));
            PerfGpuMs.Add(FPlatformTime::ToMilliseconds(RHIGetGPUFrameCycles()));
        }
        if (++PerfFrame < 128)
        {
            return true;
        }
        const auto Report = [](const TCHAR* Name, TArray<double>& Values)
        {
            Values.Sort();
            UE_LOG(LogTemp, Display, TEXT("ItemRoomPerf %s Median=%.3f P95=%.3f Max=%.3f"), Name,
                Values[Values.Num() / 2], Values[(Values.Num() * 95 - 1) / 100], Values.Last());
        };
        Report(TEXT("FrameMs"), PerfWallMs);
        Report(TEXT("GameMs"), PerfGameMs);
        Report(TEXT("RenderMs"), PerfRenderMs);
        Report(TEXT("GpuMs"), PerfGpuMs);
        for (int32 Switch = 0; Switch < 6; ++Switch)
        {
            const double Start = FPlatformTime::Seconds();
            ItemView.ShowPanel(Switch % 2 == 0 ? EBBBItemPanelPage::Character : EBBBItemPanelPage::Inventory);
            UE_LOG(LogTemp, Display, TEXT("ItemRoomPerf Switch=%d ApplyMs=%.3f"), Switch,
                (FPlatformTime::Seconds() - Start) * 1000.0);
        }
        return false;
    }

    void Configure(UBBBPlayerItemView& ItemView, UBBBPlayerItemPortrait& Portrait, const TCHAR* Request)
    {
        FString Page;
        FParse::Value(Request, TEXT("Page="), Page);
        const bool bRoom = Page.Equals(TEXT("Room"), ESearchCase::IgnoreCase);
        ExpectedPage = Page.Equals(TEXT("Character"), ESearchCase::IgnoreCase) ? EBBBItemPanelPage::Character
            : EBBBItemPanelPage::Inventory;
        ItemView.ShowPanel(ExpectedPage);
        for (const auto& Panel : ItemView.GetMenuPanels())
        {
            Panel->SetVisibility(bRoom ? EVisibility::Collapsed : EVisibility::SelfHitTestInvisible);
        }

        float PoseTime = 0.34f;
        FParse::Value(Request, TEXT("Pose="), PoseTime);
        Portrait.ReviewPoseTime = PoseTime;
        Portrait.bDirty = true;
        USpotLightComponent& Key = *Portrait.Stage->GetKey();
        FVector Position = Key.GetRelativeLocation();
        const FVector Direction = Key.GetForwardVector();
        FVector Target = Position - Direction * (Position.Z / FMath::Min(-0.01, Direction.Z));
        FParse::Value(Request, TEXT("KeyX="), Position.X);
        FParse::Value(Request, TEXT("KeyY="), Position.Y);
        FParse::Value(Request, TEXT("KeyZ="), Position.Z);
        FParse::Value(Request, TEXT("AimX="), Target.X);
        FParse::Value(Request, TEXT("AimY="), Target.Y);
        Key.SetRelativeLocation(Position);
        Key.SetRelativeRotation((Target - Position).Rotation());
        float Intensity = Key.Intensity;
        float Inner = Key.InnerConeAngle;
        float Outer = Key.OuterConeAngle;
        float Radius = Key.SourceRadius;
        FParse::Value(Request, TEXT("Key="), Intensity);
        FParse::Value(Request, TEXT("Inner="), Inner);
        FParse::Value(Request, TEXT("Outer="), Outer);
        FParse::Value(Request, TEXT("Radius="), Radius);
        Key.SetIntensity(Intensity);
        Key.SetInnerConeAngle(Inner);
        Key.SetOuterConeAngle(Outer);
        Key.SetSourceRadius(Radius);
        Key.SetSoftSourceRadius(Radius * 0.5f);

        SetRim(*Portrait.Stage, TEXT("Cool"), FVector(0, 0, 114), Request);
        SetLight(*Portrait.Stage, TEXT("Fill="), Request);
        auto* Camera = Portrait.Stage->GetCapture();
        Camera->bCameraCutThisFrame = true;
        float Exposure = Camera->PostProcessSettings.AutoExposureBias;
        float Bloom = Camera->PostProcessSettings.BloomIntensity;
        FParse::Value(Request, TEXT("Exposure="), Exposure);
        FParse::Value(Request, TEXT("Bloom="), Bloom);
        Camera->PostProcessSettings.AutoExposureBias = Exposure;
        Camera->PostProcessSettings.BloomIntensity = Bloom;

        float Mist = 0.0f;
        int32 Native = 0;
        FParse::Value(Request, TEXT("Native="), Native);
        Portrait.Stage->UpdateMist();
        auto* RoomMist = Portrait.Stage->GetMist();
        const bool bOverrideMist = FParse::Value(Request, TEXT("Mist="), Mist);
        RoomMist->SetVisibility(Native == 0 && (!bOverrideMist || Mist > 0.0f));
        if (bOverrideMist)
        {
            RoomMist->SetCustomPrimitiveDataFloat(10, Mist);
        }
        float Halo = 0.0f;
        if (FParse::Value(Request, TEXT("MistHalo="), Halo))
        {
            RoomMist->SetCustomPrimitiveDataFloat(11, Halo);
        }
        if (Native != 0 && !NativeHaze.IsValid())
        {
            auto* Fog = NewObject<UExponentialHeightFogComponent>(Portrait.Display.Get(), TEXT("ItemRoomReviewNativeFog"));
            Portrait.Display->AddInstanceComponent(Fog);
            Fog->SetupAttachment(Portrait.Display->GetRootComponent());
            Fog->SetFogDensity(0.035f);
            Fog->SetFogMaxOpacity(0.08f);
            Fog->SetFogInscatteringColor(FLinearColor::Black);
            Fog->SetVolumetricFog(true);
            Fog->SetVolumetricFogDistance(1500.0f);
            Fog->RegisterComponent();
            NativeHaze = Fog;
        }
        if (NativeHaze.IsValid())
        {
            NativeHaze->SetVisibility(Native != 0);
        }
        Key.SetVolumetricScatteringIntensity(Native != 0 ? 4.0f : 0.0f);
        Key.SetCastVolumetricShadow(Native != 0);
        const IConsoleVariable* FogSetting = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog"));
        UE_LOG(LogTemp, Display, TEXT("Item room review request %s page=%s r.VolumetricFog=%d"),
            *LastName, *Page, FogSetting ? FogSetting->GetInt() : -1);

        int32 Drag = 0;
        FParse::Value(Request, TEXT("Drag="), Drag);
        DragPhase = Drag != 0 && !bRoom && ExpectedPage == EBBBItemPanelPage::Inventory ? 0 : INDEX_NONE;
        RequestedAt = FPlatformTime::Seconds();
        CaptureAfter = Portrait.CaptureRevision + 4;
        bReadingCapture = true;
    }

    static ULightComponent* FindLevelLight(const AActor& Owner, FName Tag)
    {
        for (AActor* Actor : Owner.GetLevel()->Actors)
        {
            if (Actor && Actor->ActorHasTag(Tag))
            {
                return Actor->FindComponentByClass<ULightComponent>();
            }
        }
        return nullptr;
    }

    static void SetRim(AActor& Owner, const TCHAR* Prefix, FVector Target, const TCHAR* Request)
    {
        auto* Light = Cast<USpotLightComponent>(FindLevelLight(Owner, TEXT("InventoryRim")));
        if (!Light)
        {
            return;
        }
        const FString Key(Prefix);
        auto Read = [&](const TCHAR* Suffix, auto& Value)
        {
            return FParse::Value(Request, *(Key + Suffix + TEXT("=")), Value);
        };
        FVector Position = Light->GetRelativeLocation();
        const bool bX = Read(TEXT("X"), Position.X);
        const bool bY = Read(TEXT("Y"), Position.Y);
        const bool bZ = Read(TEXT("Z"), Position.Z);
        if (bX || bY || bZ)
        {
            Light->SetRelativeLocation(Position);
            Light->SetRelativeRotation((Target - Position).Rotation());
        }
        float Radius = Light->SourceRadius;
        if (Read(TEXT("Radius"), Radius))
        {
            Light->SetSourceRadius(Radius);
        }
        float Intensity = Light->Intensity;
        if (Read(TEXT(""), Intensity))
        {
            Light->SetIntensity(Intensity);
        }
    }

    static void SetLight(AActor& Owner, const TCHAR* Parameter, const TCHAR* Request)
    {
        float Intensity = 0.0f;
        if (FParse::Value(Request, Parameter, Intensity))
        {
            if (auto* Light = FindLevelLight(Owner, TEXT("InventoryFill")))
            {
                Light->SetIntensity(Intensity);
            }
        }
    }

    bool AwaitCapture(UBBBPlayerItemPortrait& Portrait)
    {
        UBBBPlayerItemView& ItemView = *View.Get();
        if (FPlatformTime::Seconds() - RequestedAt > 8.0)
        {
            UE_LOG(LogTemp, Error, TEXT("Item room review capture timed out: %s"), *LastName);
            Portrait.ReviewPoseTime = -1.0f;
            bReadingCapture = false;
            ++AutomaticIndex;
            return bLive;
        }
        if (Portrait.CaptureRevision < CaptureAfter || ItemView.PanelPage != ExpectedPage ||
            ItemView.BuiltPanelPage != ExpectedPage || FScreenshotRequest::IsScreenshotRequested())
        {
            return true;
        }

        if (DragPhase != INDEX_NONE)
        {
            auto* Controller = ItemView.GetItemController();
            if (DragPhase == 0)
            {
                int32 Start = INDEX_NONE;
                int32 Count = 0;
                Controller->GetStorageRange(EBBBItemType::Equipment, Start, Count);
                Dragged = Controller->GetItemDisplayData(Start).InstanceId;
            }
            if (DragPhase < 4)
            {
                SendReviewPointer(ItemView, FVector2D(188, 399), FVector2D(1558, 383), DragPhase++);
                CaptureAfter = Portrait.CaptureRevision + 1;
                return true;
            }
            const bool bMoved = Dragged.IsValid() && Controller->GetItemDisplayData(0).InstanceId == Dragged;
            UE_LOG(LogTemp, Display, TEXT("Item room review cross-panel drag %s"), bMoved ? TEXT("PASS") : TEXT("FAIL"));
            DragPhase = INDEX_NONE;
        }

        const FString File = FPaths::ConvertRelativePathToFull(Directory / (LastName + TEXT(".png")));
        FScreenshotRequest::RequestScreenshot(File, true, false, false, FIntRect(), true);
        UE_LOG(LogTemp, Display, TEXT("Item room review capture %s %.2fs"), *LastName,
            FPlatformTime::Seconds() - RequestedAt);
        bReadingCapture = false;
        ++AutomaticIndex;
        return true;
    }

    TWeakObjectPtr<UBBBPlayerItemView> View;
    TWeakObjectPtr<UExponentialHeightFogComponent> NativeHaze;
    FString Directory;
    FString LastName;
    FGuid Dragged;
    EBBBItemPanelPage ExpectedPage = EBBBItemPanelPage::Inventory;
    double ReadyDeadline = 0.0;
    double RequestedAt = 0.0;
    uint64 CaptureAfter = 0;
    int32 AutomaticIndex = 0;
    int32 DragPhase = INDEX_NONE;
    bool bLive = false;
    bool bSeeded = false;
    bool bEquipped = false;
    bool bReadingCapture = false;
    bool bPerfComplete = false;
    double PerfPreviousFrame = 0.0;
    double PerfOpenAt = 0.0;
    double PerfToggleMs = 0.0;
    uint64 PerfBeforeCapture = 0;
    int32 PerfOpenCount = 0;
    int32 PerfFrame = 0;
    TArray<double> PerfWallMs;
    TArray<double> PerfGameMs;
    TArray<double> PerfRenderMs;
    TArray<double> PerfGpuMs;
};

namespace
{
FAutoConsoleCommand CaptureItemMenu(TEXT("BBB.Player.CaptureItemMenu"),
    TEXT("Capture BBBTest inventory. With -ItemMenuReviewDir=Path apply request.txt and capture each named request."),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (FParse::Param(FCommandLine::Get(), TEXT("ItemMenuReview")))
        {
            FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
                [Review = MakeShared<FBBBItemRoomReview>()](float DeltaTime)
                {
                    return Review->Tick(DeltaTime);
                }), FParse::Param(FCommandLine::Get(), TEXT("ItemMenuPerf")) ? 0.0f : 0.06f);
        }
    }));
}

/** 通过公开控制器接口验证界面请求 不访问物品系统或写入领域状态 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBPlayerItemViewTest, "BBB.Player.ItemView.Interaction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBPlayerItemViewTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues()
        .AllowAudioPlayback(false).CreatePhysicsScene(true).CreateNavigation(false)
        .CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };

    UClass *CharacterClass = LoadClass<ABBBCharacter>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/BBBC_UA_0.BBBC_UA_0_C"));
    if (!TestNotNull(TEXT("项目玩家角色类"), CharacterClass))
    {
        return false;
    }
    ABBBCharacter *Character = World->SpawnActor<ABBBCharacter>(CharacterClass);
    ABBBPlayerController *Controller = World->SpawnActor<ABBBPlayerController>();
    if (!TestNotNull(TEXT("玩家角色"), Character) || !TestNotNull(TEXT("玩家控制器"), Controller))
    {
        return false;
    }
    ULocalPlayer *LocalPlayer = NewObject<ULocalPlayer>(GEngine);
    LocalPlayer->PlayerController = Controller;
    Controller->Player = LocalPlayer;
    Controller->Possess(Character);
    World->InitializeActorsForPlay(FURL());
    World->SetBegunPlay(true);
    Character->DispatchBeginPlay();

    Controller->SubmitItemAdd(TEXT("ModernWeapons_Rifle_01"));
    Controller->SubmitItemAdd(TEXT("ModernWeapons_Rifle_02"));
    Character->Tick(1.0f / 60.0f);
    if (!TestTrue(TEXT("角色生命周期初始化真实背包"), Controller->GetItemSlotCount() == 110
        && Controller->GetItemDisplayData(0).bOccupied && Controller->GetItemDisplayData(1).bOccupied))
    {
        return false;
    }
    const FGuid First = Controller->GetItemDisplayData(0).InstanceId;
    const FGuid Second = Controller->GetItemDisplayData(1).InstanceId;
    TestTrue(TEXT("公开身份查询与展示数据一致"), Controller->GetItemInstanceId(0) == First);
    TestFalse(TEXT("空格身份无效"), Controller->GetItemInstanceId(2).IsValid());
    TestFalse(TEXT("越界身份无效"), Controller->GetItemInstanceId(-1).IsValid());

    UBBBPlayerItemView *View = NewObject<UBBBPlayerItemView>(Controller);
    View->SetOwningPlayer(Controller);
    TestTrue(TEXT("初始化原生物品界面"), View->Initialize());
    const TSharedRef<SWidget> Widget = View->TakeWidget();
    const int32 ContextsBeforeBag = GEngine->GetWorldContexts().Num();
    const auto CaptureCount = [World]()
    {
        int32 Count = 0;
        for (TActorIterator<ASceneCapture2D> Actor(World); Actor; ++Actor)
        {
            ++Count;
        }
        return Count;
    };
    TestEqual(TEXT("打开前没有人物捕获演员"), CaptureCount(), 0);
    View->SetBackpackOpen(true);
    TestEqual(TEXT("空手打开背包默认查看第一个装备"), View->GetInspectedSlot(), 0);
    TestEqual(TEXT("默认查看不改变手持选择"), Controller->GetSelectedItemSlot(), INDEX_NONE);

    TestEqual(TEXT("玩家世界不生成捕获演员"), CaptureCount(), 0);
    TFunction<TSharedPtr<SWidget>(const TSharedRef<SWidget> &, FName)> FindTagged;
    FindTagged = [&FindTagged](const TSharedRef<SWidget> &Node, FName Tag) -> TSharedPtr<SWidget>
    {
        if (Node->GetTag() == Tag)
        {
            return Node;
        }
        FChildren *Children = Node->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            if (TSharedPtr<SWidget> Found = FindTagged(Children->GetChildAt(Index), Tag))
            {
                return Found;
            }
        }
        return nullptr;
    };
    const auto FindPanelWidget = [&FindTagged, View](FName Tag)
    {
        for (const auto& Panel : View->GetMenuPanels())
        {
            if (auto Found = FindTagged(Panel.ToSharedRef(), Tag))
            {
                return Found;
            }
        }
        return TSharedPtr<SWidget>();
    };
    const auto Click = [&](FName Tag)
    {
        const TSharedPtr<SWidget> Button = FindPanelWidget(Tag);
        if (TestTrue(FString::Printf(TEXT("导航按钮存在 %s"), *Tag.ToString()),
            Button.IsValid() && Button->GetType() == TEXT("SButton")))
        {
            StaticCastSharedPtr<SButton>(Button)->SimulateClick();
        }
    };
    TFunction<int32(const TSharedRef<SWidget> &)> CountSlots;
    CountSlots = [&CountSlots](const TSharedRef<SWidget> &Node)
    {
        int32 Count = Node->GetType() == TEXT("SBBBPlayerItemSlot") ? 1 : 0;
        FChildren *Children = Node->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            Count += CountSlots(Children->GetChildAt(Index));
        }
        return Count;
    };
    const TSharedPtr<SWidget> PanelSlots = FindPanelWidget(TEXT("Backpack.PanelSlots"));
    if (!TestTrue(TEXT("右侧物品展示面板存在"), PanelSlots.IsValid()))
    {
        return false;
    }
    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    const auto PressNumber = [&](FKey Key, int32 ExpectedSlot)
    {
        TestTrue(TEXT("数字键由背包界面处理"),
            Widget->OnPreviewKeyDown(FGeometry(), FKeyEvent(Key, FModifierKeysState(), 0, false, 0, 0)).IsEventHandled());
        Character->Tick(1.0f / 60.0f);
        TestEqual(TEXT("角色消费输入后选中当前右侧快捷位"), Controller->GetSelectedItemSlot(), ExpectedSlot);
    };
    TestEqual(TEXT("装备页同时显示三格武器与三格消耗品"), CountSlots(PanelSlots.ToSharedRef()), 6);
    TestEqual(TEXT("三个真实消耗品快捷位"), ConsumableCount, 3);
    PressNumber(EKeys::Two, 1);
    TestFalse(TEXT("普通消耗品存储位不能直接作为快捷目标"), View->SelectSlot(ConsumableStart + ConsumableCount));
    Click(TEXT("Backpack.Panel.1"));
    TestEqual(TEXT("角色页显示全部身体穿戴位置"), CountSlots(PanelSlots.ToSharedRef()), Character->GetCharacterConfig().Item.WearSlots.Num());
    const int32 BeforeCharacterKey = Controller->GetSelectedItemSlot();
    Widget->OnPreviewKeyDown(FGeometry(), FKeyEvent(EKeys::Four, FModifierKeysState(), 0, false, 0, 0));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("角色页数字键不误选武器或消耗品"),
        Controller->GetSelectedItemSlot(), BeforeCharacterKey);
    Click(TEXT("Backpack.Panel.0"));
    Click(TEXT("Backpack.Storage.3"));
    Widget->OnPreviewKeyDown(FGeometry(), FKeyEvent(EKeys::Two, FModifierKeysState(), 0, false, 0, 0));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("消耗品分类数字键选择独立目标"), Controller->GetSelectedConsumableSlot(), 1);
    Click(TEXT("Backpack.Storage.0"));
    Controller->SubmitItemSelect(INDEX_NONE);
    Character->Tick(1.0f / 60.0f);
    UBBBPlayerItemPortrait *Portrait = NewObject<UBBBPlayerItemPortrait>(Controller);
    Portrait->Prepare(Character);
    FlushAsyncLoading();
    TestTrue(TEXT("建立穿脱回归捕获"), Portrait->Prepare(Character));
    Portrait->SetActive(true);
    Controller->SubmitItemAdd(TEXT("Helmet_UASoldHelmet01"));
    Character->Tick(1.0f / 60.0f);
    const FGuid Helmet = Controller->GetItemInstanceId(20);
    TestTrue(TEXT("头盔登记真实身份"), Helmet.IsValid());
    Controller->SubmitItemMove(20, 0, Helmet);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("头盔与快捷武器交换被拒绝"), Controller->GetItemInstanceId(20), Helmet);
    TestEqual(TEXT("非法交换保留快捷武器"), Controller->GetItemInstanceId(0), First);
    TestEqual(TEXT("头盔双击目标为对应穿戴位置"), Controller->GetItemDisplayData(20).EquipSlot, 100);
    TestTrue(TEXT("双击穿戴请求接受"), View->EquipItem(20, Character, Helmet));
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f, {});
    TestTrue(TEXT("穿戴后捕获真实位置"), Controller->GetItemInstanceId(100) == Helmet);
    Controller->SubmitItemAdd(TEXT("Helmet_UASoldHelmet00"));
    Character->Tick(1.0f / 60.0f);
    const FGuid ReplacementHelmet = Controller->GetItemInstanceId(20);
    TestTrue(TEXT("双击替换头盔请求接受"), View->EquipItem(20, Character, ReplacementHelmet));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("新头盔进入穿戴位"), Controller->GetItemInstanceId(100), ReplacementHelmet);
    TestEqual(TEXT("旧头盔回到替换来源格"), Controller->GetItemInstanceId(20), Helmet);
    Controller->SubmitItemMove(100, 20, ReplacementHelmet);
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f, {});
    Controller->SubmitItemMove(100, 21, Helmet);
    Character->Tick(1.0f / 60.0f);
    Portrait->Update(1.0f, {});
    TestTrue(TEXT("脱下清空网格后捕获安全"), Controller->GetItemInstanceId(21) == Helmet
        && !Controller->GetItemInstanceId(100).IsValid());
    Portrait->Close();
    TestTrue(TEXT("控制器提供物品展示数据"), Controller->GetItemDisplayData(0).bOccupied
        && !Controller->GetItemDisplayData(0).Name.IsEmpty());
    TestFalse(TEXT("普通槽位不能由界面直接选中"), View->SelectSlot(Controller->GetQuickAccessSlotCount()));
    TestTrue(TEXT("界面提交快捷选择"), View->SelectSlot(0));
    TestEqual(TEXT("界面不提前修改选择"), Controller->GetSelectedItemSlot(), INDEX_NONE);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("界面读取角色处理后的选择"), Controller->GetSelectedItemSlot(), 0);
    TestTrue(TEXT("控制器区分实际装备状态"), Controller->GetItemDisplayData(0).bActive);
    UClass *HudClass = LoadClass<UBBBCombatHudWidget>(nullptr,
        TEXT("/Game/_Project/UI/Gameplay/WBP_CombatHUD.WBP_CombatHUD_C"));
    if (!TestNotNull(TEXT("游玩 HUD 控件蓝图"), HudClass))
    {
        return false;
    }
    auto *Hud = NewObject<UBBBCombatHudWidget>(Controller, HudClass);
    Hud->SetOwningPlayer(Controller);
    Hud->Initialize();
    const FGeometry HudGeometry = FGeometry::MakeRoot(FVector2f(1920.0f, 1080.0f), FSlateLayoutTransform());
    TestFalse(TEXT("持枪未瞄准时准星仍隐藏"), Controller->ShouldShowAimHud());
    Hud->UpdatePresentation(Controller, HudGeometry, 1.0f / 60.0f);
    const UWidget *AmmoGroup = Hud->GetWidgetFromName(TEXT("AmmoGroup"));
    const auto *LoadedLabel = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("LoadedAmmo")));
    const auto *CapacityLabel = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("CapacityAmmo")));
    if (!TestNotNull(TEXT("右下弹药组"), AmmoGroup)
        || !TestNotNull(TEXT("剩余弹药数字"), LoadedLabel) || !TestNotNull(TEXT("容量数字"), CapacityLabel))
    {
        return false;
    }
    TestTrue(TEXT("未瞄准也显示剩余弹药与容量"), AmmoGroup->IsVisible());
    int32 Loaded = 0;
    int32 Capacity = 0;
    bool bContinuous = false;
    const ABBBEquipment *Held = Cast<ABBBEquipment>(Controller->GetActiveItem());
    TestTrue(TEXT("弹药显示读取真实枪械"), Held && Held->ReadAmmoDisplay(Loaded, Capacity, bContinuous));
    TestEqual(TEXT("右下数字保持真实剩余弹药"), LoadedLabel->GetText().ToString(), FString::FromInt(Loaded));
    TestEqual(TEXT("右下数字保持真实容量"), CapacityLabel->GetText().ToString(), FString::Printf(TEXT("/%d"), Capacity));
    ABBBEquipment *HeldEquipment = Cast<ABBBEquipment>(Controller->GetActiveItem());
    FTransform Muzzle;
    if (!TestTrue(TEXT("真实手持枪械提供枪口"), HeldEquipment && HeldEquipment->TryGetMuzzleTransform(Muzzle)))
    {
        return false;
    }
    const FVector RayStart = Muzzle.GetLocation();
    const FVector RayDirection = Muzzle.GetUnitAxis(EAxis::X);
    UBBBMassSubsystem *Mass = World->GetSubsystem<UBBBMassSubsystem>();
    if (!TestNotNull(TEXT("枪口落点使用公开实体碰撞"), Mass))
    {
        return false;
    }
    FMassEntityManager &Manager = World->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const FMassArchetypeHandle AimArchetype = Manager.CreateArchetype({FTransformFragment::StaticStruct()});
    const FMassEntityHandle NearEntity = Manager.CreateEntity(AimArchetype);
    const FMassEntityHandle FarEntity = Manager.CreateEntity(AimArchetype);
    FBBBMassCollisionBody NearBody;
    NearBody.Entity = NearEntity;
    NearBody.Center = RayStart + RayDirection * 2000.0;
    NearBody.Radius = 25.0f;
    FBBBMassCollisionBody FarBody = NearBody;
    FarBody.Entity = FarEntity;
    FarBody.Center = RayStart + RayDirection * 4000.0;
    Mass->AddCollisionBody(FarBody);
    Mass->AddCollisionBody(NearBody);
    FVector AimImpact;
    TestTrue(TEXT("无表现演员的实体仍能提供枪口落点"), Controller->GetActualAimWorldPosition(AimImpact));
    TestTrue(TEXT("白圈落在最近实体表面"), AimImpact.Equals(RayStart + RayDirection * 1975.0, 0.1));
    AActor *AimWall = World->SpawnActor<AActor>();
    UBoxComponent *AimWallBox = NewObject<UBoxComponent>(AimWall);
    AimWall->SetRootComponent(AimWallBox);
    AimWallBox->SetBoxExtent(FVector(15.0, 200.0, 200.0));
    AimWallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AimWallBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    AimWallBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    AimWallBox->RegisterComponent();
    AimWall->SetActorRotation(Muzzle.GetRotation());
    AimWall->SetActorLocation(RayStart + RayDirection * 1000.0);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("墙前遮挡优先于墙后僵尸"), AimImpact.Equals(RayStart + RayDirection * 985.0, 0.1));
    AimWall->SetActorLocation(RayStart + RayDirection * 7000.0);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("僵尸后方墙面不覆盖实体落点"), AimImpact.Equals(RayStart + RayDirection * 1975.0, 0.1));
    Mass->BeginCollisionFrame();
    NearBody.Center = RayStart + RayDirection * 2300.0;
    Mass->AddCollisionBody(NearBody);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("白圈跟随实体新位置"), AimImpact.Equals(RayStart + RayDirection * 2275.0, 0.1));
    Mass->BeginCollisionFrame();
    NearBody.Center += Muzzle.GetUnitAxis(EAxis::Y) * 250.0;
    Mass->AddCollisionBody(NearBody);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("目标移出射线后白圈立即回到场景命中"), AimImpact.Equals(RayStart + RayDirection * 6985.0, 0.1));
    Mass->BeginCollisionFrame();
    AimWallBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Controller->GetActualAimWorldPosition(AimImpact);
    TestTrue(TEXT("目标与遮挡移除后不残留旧白圈位置"), AimImpact.Equals(RayStart + RayDirection * 100000.0, 0.1));
    Manager.DestroyEntity(NearEntity);
    Manager.DestroyEntity(FarEntity);
    AimWall->Destroy();
    TestTrue(TEXT("界面提交拖动交换"), View->MoveItem(0, 1, Character, First));
    TestTrue(TEXT("界面不提前改变槽位内容"), Controller->GetItemDisplayData(0).InstanceId == First);
    Character->Tick(1.0f / 60.0f);
    TestTrue(TEXT("角色完成交换"), Controller->GetItemDisplayData(0).InstanceId == Second
        && Controller->GetItemDisplayData(1).InstanceId == First);
    TestTrue(TEXT("手持装备跟随选择格内容"), Controller->GetItemDisplayData(0).bActive);

    TestFalse(TEXT("拒绝物品已换位的旧拖动"), View->MoveItem(0, 1, Character, First));
    TestFalse(TEXT("拒绝角色失效的旧拖动"), View->MoveItem(1, 0, nullptr, First));
    TestTrue(TEXT("武器移到普通格请求接受"), View->MoveItem(1, 7, Character, First));
    Character->Tick(1.0f / 60.0f);
    TestFalse(TEXT("双击拒绝过期物品身份"), View->EquipItem(7, Character, Second));
    const TSharedRef<SBBBPlayerItemSlot> WeaponSlot = SNew(SBBBPlayerItemSlot).View(View).Slot(7);
    const FPointerEvent DoubleClick(0, FVector2D::ZeroVector, FVector2D::ZeroVector,
        TSet<FKey>{EKeys::LeftMouseButton}, EKeys::LeftMouseButton, 0.0f, FModifierKeysState());
    TestTrue(TEXT("鼠标双击武器事件接受"), WeaponSlot->OnMouseButtonDoubleClick(FGeometry(), DoubleClick).IsEventHandled());
    TestEqual(TEXT("双击仍等待角色消费输入"), Controller->GetItemInstanceId(7), First);
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("双击武器进入第一个空快捷格"), Controller->GetItemInstanceId(1), First);
    TestEqual(TEXT("原一号位武器留在原地"), Controller->GetItemInstanceId(0), Second);
    TestFalse(TEXT("压入快捷栏释放原背包格"), Controller->GetItemInstanceId(7).IsValid());
    TestEqual(TEXT("压入快捷栏保留当前选择"), Controller->GetSelectedItemSlot(), 0);
    TestTrue(TEXT("压入快捷栏保留当前手持武器"), Controller->GetItemDisplayData(0).bActive);
    View->SetBackpackOpen(false);
    TestEqual(TEXT("关闭后清除玩家世界内的捕获演员"), CaptureCount(), 0);
    TestEqual(TEXT("关闭后世界上下文保持原值"), GEngine->GetWorldContexts().Num(), ContextsBeforeBag);
    TestFalse(TEXT("界面关闭后拒绝拖动"), View->MoveItem(1, 0, Character, First));
    TestFalse(TEXT("界面关闭后拒绝双击装备"), View->EquipItem(7, Character, Second));
    TestTrue(TEXT("界面提交收起"), View->SelectSlot(INDEX_NONE));
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("收起后未选择槽位"), Controller->GetSelectedItemSlot(), INDEX_NONE);
    TestNull(TEXT("收起后实际空手"), Controller->GetActiveItem());
    Hud->UpdatePresentation(Controller, HudGeometry, 1.0f / 60.0f);
    TestFalse(TEXT("空手时不显示旧枪械弹药"), AmmoGroup->IsVisible());
    TestFalse(TEXT("空手时不保留枪口落点"), Controller->GetActualAimWorldPosition(AimImpact));
    TestEqual(TEXT("快捷栏使用前三格"), Controller->GetQuickAccessSlotCount(), 3);
    TestEqual(TEXT("四类背包和身体穿戴共一百一十格"), Controller->GetItemSlotCount(), 110);
    TestEqual(TEXT("满生命显示完整白线"), Controller->GetHudHealthFraction(), 1.0f);
    TestFalse(TEXT("空手不显示瞄准界面"), Controller->ShouldShowAimHud());
    const float MaximumHealth = Character->GetCharacterConfig().MaximumHealth;
    Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{{FBBBHitContribution{MaximumHealth * 0.25f, NAME_None, Character->GetActorLocation(), FVector::ZeroVector}}});
    Character->Tick(1.0f / 60.0f);
    TestEqual(TEXT("白线读取真实生命损失"), Controller->GetHudHealthFraction(), 0.75f);
    Controller->UnPossess();
    TestFalse(TEXT("失去角色后身份无效"), Controller->GetItemInstanceId(0).IsValid());
    TestEqual(TEXT("失去角色后生命接口归零"), Controller->GetHudHealthFraction(), 0.0f);
    return true;
}

#endif
