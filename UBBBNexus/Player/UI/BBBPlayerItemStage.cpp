#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "Components/RectLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"

ABBBPlayerItemStage::ABBBPlayerItemStage()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("StageRoot")));
    Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
    Floor->SetupAttachment(GetRootComponent());
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Key = CreateDefaultSubobject<USpotLightComponent>(TEXT("ItemRoomKey"));
    Key->SetupAttachment(GetRootComponent());
    Key->SetMobility(EComponentMobility::Movable);
    Rim = CreateDefaultSubobject<USpotLightComponent>(TEXT("ItemRoomCoolRim"));
    Rim->SetupAttachment(GetRootComponent());
    Rim->SetMobility(EComponentMobility::Movable);
    Fill = CreateDefaultSubobject<URectLightComponent>(TEXT("ItemRoomFill"));
    Fill->SetupAttachment(GetRootComponent());
    Fill->SetMobility(EComponentMobility::Movable);

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

void ABBBPlayerItemStage::UpdateMist() const
{
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
