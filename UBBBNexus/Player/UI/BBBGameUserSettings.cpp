#include "BBBWork/UBBBNexus/Player/UI/BBBGameUserSettings.h"

#include "HAL/IConsoleManager.h"

void UBBBGameUserSettings::ApplyNonResolutionSettings()
{
    Super::ApplyNonResolutionSettings();

    IConsoleVariable* const Method = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
    if (ensureMsgf(Method, TEXT("[BBBGameUserSettings]Anti-aliasing console variable is unavailable")))
    {
        const int32 MethodValue = GetAntiAliasingQuality() >= 2 ? 4 : 2;
        Method->Set(MethodValue, ECVF_SetByGameSetting);
    }

    IConsoleVariable* const AnimationBudget = IConsoleManager::Get().FindConsoleVariable(TEXT("a.Budget.BudgetMs"));
    if (ensureMsgf(AnimationBudget, TEXT("[BBBGameUserSettings]Animation budget console variable is unavailable")))
    {
        constexpr float AnimationBudgets[] = { 1.0f, 1.5f, 2.0f };
        AnimationBudget->Set(AnimationBudgets[FMath::Clamp(GetViewDistanceQuality(), 0, 2)], ECVF_SetByGameSetting);
    }
}
