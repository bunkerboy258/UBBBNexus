#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationController.h"
#include "BBBWork/UBBBNexus/Appearance/BBBAppearanceComponent.h"
#include "BBBWork/UBBBNexus/Client/BBBClientSubsystem.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationWidget.h"
#include "Engine/Level.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomization, Log, All);

bool UBBBCharacterCustomizationController::IsOpen() const
{
    return Stage != nullptr;
}

bool UBBBCharacterCustomizationController::Open(APlayerController &Player)
{
    if (IsOpen() || !Player.IsLocalController() || !Player.GetPawn())
    {
        return false;
    }

    UBBBAppearanceComponent *Appearance = Player.GetPawn()->FindComponentByClass<UBBBAppearanceComponent>();
    if (!Appearance || StageLevel.IsNull() || !WidgetClass)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("换装配置不完整 Appearance=%s Stage=%s Widget=%s"),
            *GetNameSafe(Appearance), *StageLevel.ToString(), *GetNameSafe(WidgetClass.Get()));
        return false;
    }

    Draft = Appearance->GetAppearance();
    if (!Appearance->PrepareSelection(Draft))
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("当前外观无法作为完整草稿"));
        return false;
    }

    Controller = &Player;
    Target = Appearance;
    bool bLoaded = false;
    Stage = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
        Player.GetWorld(), StageLevel, StageLocation, FRotator::ZeroRotator, bLoaded);
    if (!bLoaded || !Stage)
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("展示关卡加载失败 Path=%s"), *StageLevel.ToString());
        Close();
        return false;
    }

    Stage->OnLevelShown.AddDynamic(this, &ThisClass::OnStageShown);
    return true;
}

void UBBBCharacterCustomizationController::OnStageShown()
{
    APlayerController *Player = Controller.Get();
    if (!Player || !Target.IsValid() || !Stage || !Stage->GetLoadedLevel())
    {
        Close();
        return;
    }

    //展示人物必须是本地对象 不能把真实人物当作草稿显示器
    for (AActor *Actor : Stage->GetLoadedLevel()->Actors)
    {
        if (Actor && Actor->ActorHasTag(TEXT("BBBPreview")) && !Actor->GetIsReplicated())
        {
            Preview = Actor->FindComponentByClass<UBBBAppearanceComponent>();
            break;
        }
    }
    if (!Preview.IsValid() || !Preview->PreviewAppearance(Draft))
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("展示人物缺失或蓝图组装失败"));
        Close();
        return;
    }

    Widget = CreateWidget<UBBBCharacterCustomizationWidget>(Player, WidgetClass);
    if (!Widget)
    {
        Close();
        return;
    }

    PreviousViewTarget = Player->GetViewTarget();
    bPreviousCursor = Player->bShowMouseCursor;
    if (UBBBPlayerInputSystem *InputAdapter = Player->FindComponentByClass<UBBBPlayerInputSystem>())
    {
        bPreviousGameplayInput = InputAdapter->IsInputEnabled();
        InputAdapter->SetInputEnabled(false);
    }
    bInputCaptured = true;
    Widget->Customization = this;
    Widget->AddToPlayerScreen();
    Player->SetIgnoreMoveInput(true);
    Player->SetIgnoreLookInput(true);
    Player->bShowMouseCursor = true;
    FInputModeUIOnly Input;
    Input.SetWidgetToFocus(Widget->TakeWidget());
    Player->SetInputMode(Input);
    SelectView(TEXT("BBBPreviewMain"));
    UE_LOG(LogBBBCustomization, Log, TEXT("换装界面已打开"));
}

void UBBBCharacterCustomizationController::Close()
{
    if (Widget)
    {
        Widget->RemoveFromParent();
        Widget = nullptr;
    }

    //只撤销本次增加的忽略计数 不清除其它菜单或玩法持有的输入锁
    if (APlayerController *Player = Controller.Get(); Player && bInputCaptured)
    {
        Player->SetIgnoreMoveInput(false);
        Player->SetIgnoreLookInput(false);
        Player->bShowMouseCursor = bPreviousCursor;
        if (UBBBPlayerInputSystem *InputAdapter = Player->FindComponentByClass<UBBBPlayerInputSystem>())
        {
            InputAdapter->SetInputEnabled(bPreviousGameplayInput);
        }
        if (PreviousViewTarget.IsValid())
        {
            Player->SetViewTarget(PreviousViewTarget.Get());
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

    if (Stage)
    {
        Stage->OnLevelShown.RemoveAll(this);
        Stage->SetShouldBeVisible(false);
        Stage->SetShouldBeLoaded(false);
        Stage->SetIsRequestingUnloadAndRemoval(true);
        Stage = nullptr;
    }
    bInputCaptured = false;
    Preview.Reset();
    Target.Reset();
    Controller.Reset();
    PreviousViewTarget.Reset();
    Draft = {};
}

FBBBAppearanceSelection UBBBCharacterCustomizationController::GetDraft() const
{
    return Draft;
}

bool UBBBCharacterCustomizationController::SetDraft(FBBBAppearanceSelection Selection)
{
    if (!Target.IsValid() || !Preview.IsValid()
        || !Target->PrepareSelection(Selection) || !Preview->PreviewAppearance(Selection))
    {
        UE_LOG(LogBBBCustomization, Warning, TEXT("草稿未通过配置检查或预览组装失败"));
        return false;
    }
    Draft = MoveTemp(Selection);
    return true;
}

bool UBBBCharacterCustomizationController::CycleItem(const FName Slot, const int32 Direction)
{
    if (!Target.IsValid() || Direction == 0)
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

bool UBBBCharacterCustomizationController::SetColor(const FName Slot, const int32 Region, const FLinearColor Color)
{
    if (Region < 0 || Region >= 8)
    {
        return false;
    }
    FBBBAppearanceSelection Selection = Draft;
    for (FBBBAppearancePart &Part : Selection.Parts)
    {
        if (Part.Slot == Slot)
        {
            //颜色按连续区域记录 不能为跳过的区域生成未初始化数值或覆盖材质默认颜色
            if (Region > Part.Colors.Num())
            {
                UE_LOG(LogBBBCustomization, Warning, TEXT("颜色区域缺少前序配置 Slot=%s Region=%d"), *Slot.ToString(), Region);
                return false;
            }
            if (Region == Part.Colors.Num())
            {
                Part.Colors.Add(Color);
            }
            Part.Colors[Region] = Color;
            return SetDraft(MoveTemp(Selection));
        }
    }
    return false;
}

bool UBBBCharacterCustomizationController::SetSurface(const float Dirt, const float Weathering)
{
    FBBBAppearanceSelection Selection = Draft;
    Selection.Dirt = Dirt;
    Selection.Weathering = Weathering;
    return SetDraft(MoveTemp(Selection));
}

bool UBBBCharacterCustomizationController::Apply()
{
    if (!Target.IsValid() || !Controller.IsValid()
        || Target->GetOwner() != Controller->GetPawn() || !Target->CommitAppearance(Draft))
    {
        UE_LOG(LogBBBCustomization, Warning, TEXT("应用失败 目标已变化或蓝图组装失败"));
        return false;
    }

    ULocalPlayer *Player = Controller->GetLocalPlayer();
    UBBBClientSubsystem *Client = Player ? Player->GetSubsystem<UBBBClientSubsystem>() : nullptr;
    if (!Client || !Client->SaveAppearance(Draft))
    {
        UE_LOG(LogBBBCustomization, Error, TEXT("外观已应用 但本地保存失败"));
        return false;
    }
    UE_LOG(LogBBBCustomization, Log, TEXT("外观已应用并保存"));
    return true;
}

void UBBBCharacterCustomizationController::SelectView(const FName CameraTag)
{
    if (!Controller.IsValid() || !Stage || !Stage->GetLoadedLevel())
    {
        return;
    }
    for (AActor *Actor : Stage->GetLoadedLevel()->Actors)
    {
        if (Actor && Actor->ActorHasTag(CameraTag))
        {
            Controller->SetViewTargetWithBlend(Actor, 0.3f);
            return;
        }
    }
    UE_LOG(LogBBBCustomization, Warning, TEXT("展示机位不存在 Tag=%s"), *CameraTag.ToString());
}

void UBBBCharacterCustomizationController::RotatePreview(const float Degrees)
{
    if (Preview.IsValid() && FMath::IsFinite(Degrees))
    {
        Preview->GetOwner()->AddActorWorldRotation(FRotator(0.0f, Degrees, 0.0f));
    }
}
