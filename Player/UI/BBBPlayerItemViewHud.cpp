#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/SBBBCombatHud.h"

TSharedRef<SWidget> UBBBPlayerItemView::MakeGameplayHud()
{
    return SNew(SBBBCombatHud).View(this);
}
