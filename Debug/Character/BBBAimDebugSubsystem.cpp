#include "BBBAimDebugSubsystem.h"
#include "AimIKDebug.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

namespace
{
    int32 Enabled = 0;
#if !UE_BUILD_SHIPPING
    FAutoConsoleVariableRef EnableCommand(TEXT("bbb.AimDebug"), Enabled, TEXT("本地瞄准射击调试 0关闭 1开启"));
    FAutoConsoleCommandWithWorld ClearCommand(TEXT("bbb.AimDebug.Clear"), TEXT("清除瞄准调试记录"),
        FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
        {
            if (World)
            {
                if (auto* Debug = World->GetSubsystem<UBBBAimDebugSubsystem>())
                {
                    Debug->Clear();
                }
            }
        }));
#endif
    void Label(UWorld* World, const FVector& Position, const FString& Text, FColor Color)
    {
        DrawDebugString(World, Position, Text, nullptr, Color, 0, false, 0.8f);
    }
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
    Clear();
    Super::Deinitialize();
}

void UBBBAimDebugSubsystem::Clear()
{
    FAimIKDebug::SetEnabled(SelectedMesh.Get(), false);
    SelectedMesh.Reset();
    SelectedWeapon.Reset();
    Shots.Reset();
}

void UBBBAimDebugSubsystem::RecordLaunch(UWorld* World, APawn* Pawn, FMassEntityHandle Entity,
    const FTransform& Muzzle, const FVector& Direction, const FGuid& AttackId)
{
    if (!Enabled || !World || !Pawn || !Pawn->IsLocallyControlled())
    {
        return;
    }
    auto* Debug = World->GetSubsystem<UBBBAimDebugSubsystem>();
    const auto* Character = Cast<ABBBCharacter>(Pawn);
    if (!Debug || !Character || Debug->SelectedMesh.Get() != Character->GetMesh()
        || Debug->SelectedWeapon.Get() != Character->GetHeldEquipment())
    {
        return;
    }
    if (Debug->Shots.Num() >= 128)
    {
        Debug->Shots.RemoveAt(0);
    }
    auto& Shot = Debug->Shots.AddDefaulted_GetRef();
    Shot.Entity = Entity;
    Shot.Sequence = ++Debug->Sequence;
    Shot.AttackId = AttackId;
    Shot.Expires = World->GetTimeSeconds() + 3;
    Shot.Muzzle = Muzzle;
    Shot.Direction = Direction;
}

void UBBBAimDebugSubsystem::RecordHit(UWorld* World, FMassEntityHandle Entity, const FVector& Position, const FVector& Normal)
{
    if (!Enabled || !World)
    {
        return;
    }
    if (auto* Debug = World->GetSubsystem<UBBBAimDebugSubsystem>())
    {
        auto* Shot = Debug->Shots.FindByPredicate([Entity](const auto& Entry) { return Entry.Entity == Entity; });
        if (Shot && Shot->Expires >= World->GetTimeSeconds() && Shot->Hits.Num() < 16)
        {
            Shot->Hits.Emplace(Position, Normal);
        }
    }
}

void UBBBAimDebugSubsystem::Draw(UWorld* World, ELevelTick Tick, float Delta)
{
    if (World != GetWorld())
    {
        return;
    }
    if (!Enabled)
    {
        if (!SelectedMesh.IsExplicitlyNull() || !SelectedWeapon.IsExplicitlyNull() || !Shots.IsEmpty())
        {
            Clear();
        }
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
    auto* Mesh = Character ? Character->GetMesh() : nullptr;
    if (SelectedMesh.IsStale() || SelectedWeapon.IsStale()
        || SelectedMesh.Get() != Mesh || SelectedWeapon.Get() != Weapon || !Enabled)
    {
        Clear();
        SelectedMesh = Mesh;
        SelectedWeapon = Weapon;
        if (Weapon && Mesh)
        {
            FAimIKDebug::SetEnabled(Mesh, true);
        }
    }
    if (!Enabled || !Mesh || !Weapon)
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
    Label(World, Ideal + FVector(0, 0, 12), TEXT("Ideal"), FColor::Green);
    DrawDebugCoordinateSystem(World, Muzzle.GetLocation(), Muzzle.Rotator(), 15);
    Label(World, Muzzle.GetLocation() + FVector(0, 0, 5), TEXT("FinalMuzzle"), FColor::White);
    FVector Prediction;
    if (Player->GetActualAimWorldPosition(Prediction))
    {
        DrawDebugLine(World, Muzzle.GetLocation(), Prediction, FColor::Orange);
        DrawDebugSphere(World, Prediction, 6, 16, FColor::Orange);
        Label(World, Prediction + FVector(0, 0, 24), TEXT("MuzzlePrediction"), FColor::Orange);
    }
    FAimIKDebugFrame Frame;
    if (FAimIKDebug::Read(Mesh, Frame))
    {
        const FVector AppliedDirection = Frame.Applied.TransformVectorNoScale(Frame.Axis).GetSafeNormal();
        const float Difference = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(
            FVector::DotProduct(AppliedDirection, Muzzle.GetUnitAxis(EAxis::X)), -1.0, 1.0)));
        Label(World, Frame.FollowTarget + FVector(0, 0, 36), TEXT("FollowTarget"), FColor::Cyan);
        Label(World, Frame.SolveTarget + FVector(0, 0, 48), TEXT("SolveTarget"), FColor::Purple);
        Label(World, Muzzle.GetLocation() + FVector(0, 0, 35), FString::Printf(
            TEXT("Eval %llu Frame %llu Alpha %.2f Residual %.2fdeg FinalDelta %.2fdeg Pellet %llu"),
            Frame.Evaluation, Frame.Frame, Frame.Alpha, Frame.ResidualDegrees, Difference, Sequence), FColor::White);
    }
    else
    {
        Label(World, Muzzle.GetLocation() + FVector(0, 0, 35), TEXT("AimIK: no current-frame evaluation"), FColor::Silver);
    }
    const double Now = World->GetTimeSeconds();
    Shots.RemoveAll([Now](const auto& Shot) { return Shot.Expires < Now; });
    for (const auto& Shot : Shots)
    {
        const FString Identity = FString::Printf(TEXT("P%llu Shot %s Entity %d:%d"), Shot.Sequence,
            *Shot.AttackId.ToString(EGuidFormats::Short), Shot.Entity.Index, Shot.Entity.SerialNumber);
        DrawDebugDirectionalArrow(World, Shot.Muzzle.GetLocation(), Shot.Muzzle.GetLocation() + Shot.Direction * 250,
            10, FColor::Yellow, false, -1, 0, 1);
        Label(World, Shot.Muzzle.GetLocation(), Identity + TEXT(" Launch"), FColor::Yellow);
        for (const auto& Hit : Shot.Hits)
        {
            DrawDebugSolidBox(World, Hit.Key, FVector(2), FColor::Red);
            DrawDebugDirectionalArrow(World, Hit.Key, Hit.Key + Hit.Value * 20, 4, FColor::Red);
            DrawDebugLine(World, Shot.Muzzle.GetLocation(), Hit.Key, FColor::Red);
            Label(World, Hit.Key + FVector(0, 0, 6), Identity + TEXT(" TrueImpact"), FColor::Red);
        }
    }
}
