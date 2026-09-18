#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterDiscreteMontageInput.generated.h"

class UAnimMontage;

/** 蒙太奇表现输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteMontageInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PlayRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Sequence = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bReload = false;
};
