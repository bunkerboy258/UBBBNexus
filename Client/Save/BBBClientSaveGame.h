#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BBBWork/UBBBNexus/Customization/Appearance/Data/BBBAppearanceSelection.h"
#include "BBBClientSaveGame.generated.h"

/** 本地玩家配置的存档边界 */
UCLASS()
class ABBB_EVAC_API UBBBClientSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    /** 最近一次应用的外观 */
    UPROPERTY(SaveGame)
    FBBBAppearanceSelection Appearance;
};
