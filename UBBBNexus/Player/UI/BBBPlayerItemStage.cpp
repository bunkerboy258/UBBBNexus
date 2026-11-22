#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "Animation/AnimSequence.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SpotLight.h"
#include "UObject/ConstructorHelpers.h"

ABBBPlayerItemStage::ABBBPlayerItemStage()
{
    static ConstructorHelpers::FObjectFinder<UAnimSequence> Armed(
        TEXT("/Game/_Project/Characters/Animation/UsedAnimation/Continuous/Idle/MM_Rifle_IdleBreak_Scan_Inventory.MM_Rifle_IdleBreak_Scan_Inventory"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> Unarmed(
        TEXT("/Game/_Project/Characters/Animation/UsedAnimation/Continuous/Idle/M_Neutral_Stand_Idle_Loop.M_Neutral_Stand_Idle_Loop"));
    ArmedIdle = Armed.Object;
    UnarmedIdle = Unarmed.Object;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("StageRoot")));
    Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
    Floor->SetupAttachment(GetRootComponent());
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Mist = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mist"));
    Mist->SetupAttachment(GetRootComponent());
    Mist->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mist->SetCastShadow(false);
    Mist->SetTranslucentSortPriority(-1);

    Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("RoomCapture"));
    Capture->SetupAttachment(GetRootComponent());
    Capture->bCaptureEveryFrame = false;
    Capture->bCaptureOnMovement = false;
    Capture->bAlwaysPersistRenderingState = true;
    Capture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;

    CharacterAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterAnchor"));
    CharacterAnchor->SetupAttachment(GetRootComponent());
    OrbitAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("OrbitAnchor"));
    OrbitAnchor->SetupAttachment(CharacterAnchor);
}

void ABBBPlayerItemStage::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    UpdateMist();
}

void ABBBPlayerItemStage::PostRegisterAllComponents()
{
    Super::PostRegisterAllComponents();
    UpdateMist();
}

USpotLightComponent* ABBBPlayerItemStage::GetKey() const
{
    return IsValid(MistKeyLight) ? MistKeyLight->SpotLightComponent : nullptr;
}

void ABBBPlayerItemStage::UpdateMist() const
{
    const USpotLightComponent* Key = GetKey();
    if (!Key)
    {
        Mist->SetCustomPrimitiveDataFloat(3, 0.0f);
        return;
    }
    const FVector FogCenter = Mist->CalcBounds(Mist->GetComponentTransform()).Origin;
    const FVector LightOrigin = Key->GetComponentLocation() - FogCenter;
    const double OuterCos = FMath::Cos(FMath::DegreesToRadians(MistOuterAngle));
    const double InnerCos = FMath::Cos(FMath::DegreesToRadians(FMath::Min(MistInnerAngle, MistOuterAngle)));
    Mist->SetCustomPrimitiveDataVector4(0, FVector4(LightOrigin, Key->Intensity));
    Mist->SetCustomPrimitiveDataVector4(4, FVector4(Key->GetForwardVector(), OuterCos));
    Mist->SetCustomPrimitiveDataVector4(8, FVector4(InnerCos, Key->AttenuationRadius, MistDensity, MistHalo));
}

void ABBBPlayerItemStage::RefreshEditorPreview()
{
    UpdateMist();
    Capture->bCameraCutThisFrame = true;
    Capture->CaptureScene();
}
