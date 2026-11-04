#include "BBBAimDebugSubsystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

namespace
{
    int32 Enabled = 0;
#if !UE_BUILD_SHIPPING
    FAutoConsoleVariableRef EnableCommand(TEXT("bbb.AimDebug"), Enabled, TEXT("本地瞄准射击调试 0关闭 1开启"));
#endif
}

bool UBBBAimDebugSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    const auto* World = Cast<UWorld>(Outer);
    return !UE_BUILD_SHIPPING && World && World->IsGameWorld() && Super::ShouldCreateSubsystem(Outer);
}

void UBBBAimDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PostTick = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UBBBAimDebugSubsystem::Draw);
}

void UBBBAimDebugSubsystem::Deinitialize()
{
    FWorldDelegates::OnWorldPostActorTick.Remove(PostTick);
    Super::Deinitialize();
}

void UBBBAimDebugSubsystem::Draw(UWorld* World, ELevelTick Tick, float Delta)
{
    if (World != GetWorld())
    {
        return;
    }
    if (!Enabled)
    {
        return;
    }
    ABBBPlayerController* Player = nullptr;
    if (Enabled)
    {
        for (auto It = World->GetPlayerControllerIterator(); It; ++It)
        {
            auto* Candidate = Cast<ABBBPlayerController>(It->Get());
            if (Candidate && Candidate->IsLocalController())
            {
                Player = Candidate;
                break;
            }
        }
    }
    auto* Character = Player ? Cast<ABBBCharacter>(Player->GetPawn()) : nullptr;
    auto* Weapon = Character ? Character->GetHeldEquipment() : nullptr;
    if (!Weapon)
    {
        return;
    }
    FTransform Muzzle;
    if (!Weapon->TryGetMuzzleTransform(Muzzle))
    {
        return;
    }
    const FVector Ideal = Character->RuntimeData.Aim.ReadAimState().AimTargetWorld;
    DrawDebugSphere(World, Ideal, 10, 16, FColor::Green);
    FVector Prediction;
    if (Player->GetActualAimWorldPosition(Prediction))
    {
        DrawDebugLine(World, Muzzle.GetLocation(), Prediction, FColor::Orange);
        DrawDebugSphere(World, Prediction, 6, 16, FColor::Orange);
    }
}
