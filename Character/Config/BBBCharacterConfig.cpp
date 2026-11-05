#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UBBBCharacterConfig::UBBBCharacterConfig()
{
    static ConstructorHelpers::FObjectFinder<USoundBase> PickupSound(
        TEXT("/Game/_Project/Test/V01/Audio/Click.Click"));
    Pickup.PickupSound = PickupSound.Object;
}
