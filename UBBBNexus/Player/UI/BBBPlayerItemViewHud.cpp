#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudWidget.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBExplorationHud.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UBBBPlayerItemView::MakeGameplayHud()
{
    if (!CombatHud && CombatHudClass && GetOwningPlayer())
    {
        CombatHud = CreateWidget<UBBBCombatHudWidget>(GetOwningPlayer(), CombatHudClass);
        if (CombatHud)
        {
            CombatHud->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }
    return CombatHud ? CombatHud->TakeWidget() : SNullWidget::NullWidget;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeExplorationHud()
{
    return SNew(SBBBExplorationHud).View(this);
}

void UBBBPlayerItemView::UpdateExplorationPresentation()
{
    if (EvacPresentationWorld.Get() != GetWorld())
    {
        EvacPresentationWorld = GetWorld();
        LastEvacCompletionAt = -1.0;
    }
    const auto *Controller = GetItemController();
    const double CompletedAt = Controller ? Controller->GetExplorationCompletionTime() : -1.0;
    if (CompletedAt < 0.0 || CompletedAt == LastEvacCompletionAt)
    {
        return;
    }
    LastEvacCompletionAt = CompletedAt;
    const double Age = FPlatformTime::Seconds() - CompletedAt;
    if (Age >= 0.0 && Age < 3.0 && EvacSound)
    {
        UGameplayStatics::PlaySound2D(this, EvacSound);
    }
}
