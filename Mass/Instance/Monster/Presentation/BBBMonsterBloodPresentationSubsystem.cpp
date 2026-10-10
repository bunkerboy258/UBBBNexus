#include "BBBMonsterBloodPresentationSubsystem.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBloodPresentationDefinition.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Math/RotationMatrix.h"
#include "NiagaraDataChannel.h"
#include "NiagaraDataChannelAccessor.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelHandler.h"
#include "NiagaraDataChannelData.h"
#include "Misc/App.h"
#include "HAL/PlatformTime.h"

void UBBBMonsterBloodPresentationSubsystem::Deinitialize()
{
    for (UDecalComponent* Decal : Decals)
    {
        if (Decal)
        {
            Decal->DestroyComponent();
        }
    }
    Decals.Reset();
    Residues.Reset();
    Droplets.Reset();
    Super::Deinitialize();
}

void UBBBMonsterBloodPresentationSubsystem::Publish(const UBBBMonsterBloodPresentationDefinition& Settings, TConstArrayView<FBBBMonsterBloodImpact> Impacts)
{
    UWorld* World = GetWorld();
    if (Impacts.IsEmpty() || World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }
    if (!ensureMsgf(Settings.ImpactChannel && Settings.ImpactChannel->Get(), TEXT("[BBBMonsterBlood]血效配置不完整")))
    {
        return;
    }

    UNiagaraDataChannel* Channel = Settings.ImpactChannel->Get();
    auto* Handler = UNiagaraDataChannelLibrary::FindDataChannelHandler(World, Channel);
    if (!ensureMsgf(Channel && Handler, TEXT("[BBBMonsterBlood]通道处理器缺失")))
    {
        return;
    }
    TMap<FNiagaraDataChannelData*, TArray<int32>> Batches;
    for (int32 Index = 0; Index < Impacts.Num(); ++Index)
    {
        FNDCAccessContextInst Access(Channel->GetAccessContextType());
        Access.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(Impacts[Index].Position);
        auto Data = Handler->FindData(Access, ENiagaraResourceAccess::WriteOnly);
        if (ensureMsgf(Data.IsValid(), TEXT("[BBBMonsterBlood]空间岛缺失")))
        {
            Batches.FindOrAdd(Data.Get()).Add(Index);
        }
    }
    for (const auto& Batch : Batches)
    {
        FNDCAccessContextInst Access(Channel->GetAccessContextType());
        Access.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(Impacts[Batch.Value[0]].Position);
        auto* Writer = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel_WithContext(World,
            Settings.ImpactChannel, Access, Batch.Value.Num(), false, true, false, TEXT("BBBMonsterBlood"));
        if (ensureMsgf(Writer, TEXT("[BBBMonsterBlood]批量血粒子通道写入失败")))
        {
            for (int32 Index = 0; Index < Batch.Value.Num(); ++Index)
            {
                const auto& Impact = Impacts[Batch.Value[Index]];
                Writer->WritePosition(TEXT("Position"), Index, Impact.Position);
                const FVector Outward = Impact.Normal.GetSafeNormal();
                const FVector Tangent = FVector::VectorPlaneProject(Impact.Direction, Outward).GetSafeNormal();
                const FVector Spray = (Outward * 0.75f + Tangent * 0.25f + FVector::UpVector * 0.25f).GetSafeNormal();
                Writer->WriteVector(TEXT("Normal"), Index, Outward);
                Writer->WriteVector(TEXT("Direction"), Index, Spray);
                Writer->WriteInt(TEXT("Surface"), Index, Impact.bSevering ? 3 : 2);
            }
        }
    }
    for (const FBBBMonsterBloodImpact& Impact : Impacts)
    {
        EmitDroplets(Settings, Impact);
    }
}

void UBBBMonsterBloodPresentationSubsystem::EmitDroplets(const UBBBMonsterBloodPresentationDefinition& Settings, const FBBBMonsterBloodImpact& Impact)
{
    if (Settings.SplatterMaterials.IsEmpty() || Settings.DropletMaterials.IsEmpty())
    {
        return;
    }

    const FVector Outward = Impact.Normal.GetSafeNormal();
    const FVector Spray = (Outward * 0.75f + FVector::VectorPlaneProject(Impact.Direction, Outward).GetSafeNormal() * 0.25f
        + FVector::UpVector * 0.25f).GetSafeNormal();
    const FVector Tangent = FVector::CrossProduct(Spray, FMath::Abs(Spray.Z) > 0.95f ? FVector::ForwardVector : FVector::UpVector).GetSafeNormal();
    const FVector Bitangent = FVector::CrossProduct(Spray, Tangent);
    FRandomStream Random(Impact.Seed ^ GetTypeHash(Impact.Position));
    const int32 Count = Impact.bSevering ? 6 : Random.RandRange(2, 3);
    for (int32 Index = 0; Index < Count && Droplets.Num() < FMath::Clamp(Settings.MaximumFlights, 3, 128); ++Index)
    {
        const float Angle = Random.FRandRange(0.0f, 2.0f * PI);
        const FVector Spread = Tangent * FMath::Cos(Angle) + Bitangent * FMath::Sin(Angle);
        FBBBMonsterBloodDroplet Droplet;
        Droplet.Settings = &Settings;
        Droplet.Position = Impact.Position + Outward * 6.0f;
        Droplet.Velocity = Spray * Random.FRandRange(520.0f, 800.0f) + Spread * Random.FRandRange(80.0f, 220.0f);
        Droplet.Seed = Random.GetUnsignedInt();
        Droplet.bFine = Index > 0;
        if (Impact.bSevering && Index >= 3)
        {
            Droplet.Velocity = FVector(0.0f, 0.0f, -80.0f) + Spread * 45.0f;
            Droplet.Age = -0.12f * static_cast<float>(Index - 2);
        }
        Droplets.Add(Droplet);
    }
}

void UBBBMonsterBloodPresentationSubsystem::AdvancePresentation(float DeltaSeconds)
{
    const double Started = FPlatformTime::Seconds();
    UWorld* World = GetWorld();
    if (!World || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f)
    {
        return;
    }

    int32 TraceBudget = 8;
    for (const auto& Droplet : Droplets)
    {
        if (const auto* Settings = Droplet.Settings.Get())
        {
            TraceBudget = FMath::Max(TraceBudget, FMath::Clamp(Settings->MaximumTracesPerFrame, 8, 256));
        }
    }

    FCollisionObjectQueryParams Objects(ECC_WorldStatic);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterBloodFlight), true);
    const float Step = FMath::Min(DeltaSeconds, 0.05f);
    const int32 InitialBudget = TraceBudget;
    for (int32 Index = Droplets.Num() - 1; Index >= 0; --Index)
    {
        auto& Droplet = Droplets[Index];
        Droplet.Age += DeltaSeconds;
        if (!Droplet.Settings.IsValid() || Droplet.Age > 2.0f || TraceBudget <= 0)
        {
            Droplets.RemoveAtSwap(Index);
            continue;
        }
        if (Droplet.Age < 0.0f)
        {
            continue;
        }

        const FVector End = Droplet.Position + Droplet.Velocity * Step - FVector::UpVector * (490.0f * Step * Step);
        FHitResult Contact;
        --TraceBudget;
        if (World->LineTraceSingleByObjectType(Contact, Droplet.Position, End, Objects, Params))
        {
            PlaceResidue(Droplet, Contact, TraceBudget);
            Droplets.RemoveAtSwap(Index);
            continue;
        }

        Droplet.Position = End;
        Droplet.Velocity.Z -= 980.0f * Step;
    }

    const double Now = World->GetTimeSeconds();
    for (int32 Index = 0; Index < Decals.Num(); ++Index)
    {
        if (!Residues[Index].Surface.IsValid() || Now - Residues[Index].CreatedAt > Residues[Index].Lifetime + 4.0f)
        {
            Decals[Index]->SetVisibility(false);
        }
    }
    LastTraceCount = InitialBudget - TraceBudget;
    LastAdvanceMilliseconds = (FPlatformTime::Seconds() - Started) * 1000.0;
}

void UBBBMonsterBloodPresentationSubsystem::PlaceResidue(const FBBBMonsterBloodDroplet& Droplet, const FHitResult& Contact, int32& TraceBudget)
{
    const auto* Settings = Droplet.Settings.Get();
    UPrimitiveComponent* Surface = Contact.GetComponent();
    if (!Settings || !Surface || !Surface->bReceivesDecals || Contact.bStartPenetrating
        || Cast<USkeletalMeshComponent>(Surface) || Cast<APawn>(Contact.GetActor()))
    {
        return;
    }

    UWorld* World = GetWorld();
    const double Now = World->GetTimeSeconds();
    const FVector Normal = Contact.ImpactNormal.GetSafeNormal();
    int32 Nearby = 0;
    int32 NearbyPool = INDEX_NONE;
    int32 NearbyFine = 0;
    for (int32 Index = 0; Index < Decals.Num(); ++Index)
    {
        if (Residues[Index].Surface.Get() == Surface && Now - Residues[Index].CreatedAt < Residues[Index].Lifetime
            && FVector::DistSquared(Decals[Index]->GetComponentLocation(), Contact.ImpactPoint) < FMath::Square(Settings->AccumulationRadius)
            && FVector::DotProduct(Residues[Index].Normal, Normal) > 0.95f)
        {
            if (Residues[Index].Kind == 1)
            {
                ++NearbyFine;
            }
            if (Residues[Index].Kind != 1)
            {
                ++Nearby;
            }
            if (Residues[Index].Kind == 2)
            {
                NearbyPool = Index;
            }
        }
    }

    if (Droplet.bFine && NearbyFine >= Settings->LocalDecalLimit * 2)
    {
        return;
    }

    if (!Droplet.bFine && NearbyPool != INDEX_NONE && Nearby >= Settings->LocalDecalLimit)
    {
        auto& Residue = Residues[NearbyPool];
        Residue.Coverage = FMath::Min(1.0f, Residue.Coverage + 0.12f);
        Residue.CreatedAt = Now;
        auto* Material = Cast<UMaterialInstanceDynamic>(Decals[NearbyPool]->GetDecalMaterial());
        if (ensure(Material))
        {
            Material->SetScalarParameterValue(TEXT("Coverage"), Residue.Coverage);
            Material->SetScalarParameterValue(TEXT("SpawnTime"), Now);
        }
        Decals[NearbyPool]->SetFadeOut(Residue.Lifetime, 4.0f, false);
        Decals[NearbyPool]->SetLifeSpan(0.0f);
        return;
    }

    const uint8 Kind = Droplet.bFine ? 1 : Nearby >= Settings->LocalDecalLimit - 1 && !Settings->PoolMaterials.IsEmpty() ? 2 : 0;
    const auto& Materials = Kind == 1 ? Settings->DropletMaterials : Kind == 2 ? Settings->PoolMaterials : Settings->SplatterMaterials;
    if (Materials.IsEmpty() || (!Droplet.bFine && Nearby >= Settings->LocalDecalLimit))
    {
        return;
    }

    FRandomStream Random(Droplet.Seed);
    UMaterialInterface* Source = Materials[Random.RandRange(0, Materials.Num() - 1)];
    if (!Source)
    {
        return;
    }

    UTexture* Mask = nullptr;
    Source->GetTextureParameterValue(FMaterialParameterInfo(TEXT("BloodMask")), Mask);
    const auto* Texture = Cast<UTexture2D>(Mask);
    const float Aspect = Texture ? FMath::Clamp(static_cast<float>(Texture->GetSizeX()) / FMath::Max(1, Texture->GetSizeY()), 0.5f, 3.0f) : 1.0f;
    float HalfLength = Kind == 1 ? Random.FRandRange(10.0f, 24.0f) : Random.FRandRange(30.0f, 48.0f);
    const float WidthRatio = Kind == 0 ? Random.FRandRange(0.4f, 0.65f) : Random.FRandRange(0.6f, 0.9f);
    float HalfWidth = HalfLength / Aspect * WidthRatio;
    FVector Axis = FVector::VectorPlaneProject(Droplet.Velocity, Normal).GetSafeNormal();
    if (Axis.IsNearlyZero())
    {
        Axis = FVector::CrossProduct(Normal, FMath::Abs(Normal.Z) > 0.95f ? FVector::ForwardVector : FVector::UpVector).GetSafeNormal();
    }
    Axis = Axis.RotateAngleAxis(Random.FRandRange(-18.0f, 18.0f), Normal);
    const FVector Side = FVector::CrossProduct(Normal, Axis).GetSafeNormal();

    FCollisionObjectQueryParams Objects(ECC_WorldStatic);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterBloodEdge), true);
    bool bSupported = false;
    for (int32 Attempt = 0; Attempt < 2 && !bSupported; ++Attempt)
    {
        bSupported = true;
        for (const FVector& Offset : {Axis * HalfLength + Side * HalfWidth, Axis * HalfLength - Side * HalfWidth,
            -Axis * HalfLength + Side * HalfWidth, -Axis * HalfLength - Side * HalfWidth})
        {
            if (TraceBudget <= 0)
            {
                return;
            }
            --TraceBudget;
            FHitResult Edge;
            const FVector Point = Contact.ImpactPoint + Offset;
            if (!World->LineTraceSingleByObjectType(Edge, Point + Normal * 3.0f, Point - Normal * 3.0f, Objects, Params)
                || Edge.GetComponent() != Surface || FVector::DotProduct(Edge.ImpactNormal, Normal) < 0.95f)
            {
                bSupported = false;
                break;
            }
        }
        if (!bSupported)
        {
            HalfLength *= 0.5f;
            HalfWidth *= 0.5f;
        }
    }
    if (!bSupported)
    {
        return;
    }

    int32 Slot = INDEX_NONE;
    double Oldest = TNumericLimits<double>::Max();
    for (int32 Index = 0; Index < Decals.Num(); ++Index)
    {
        if (Residues[Index].CreatedAt + Residues[Index].Lifetime < Oldest)
        {
            Oldest = Residues[Index].CreatedAt + Residues[Index].Lifetime;
            Slot = Index;
        }
    }
    if (Decals.Num() < FMath::Clamp(Settings->MaximumDecals, 1, 256) && Oldest > Now)
    {
        auto* Decal = NewObject<UDecalComponent>(this);
        Decal->SetFadeScreenSize(0.002f);
        Decal->RegisterComponentWithWorld(World);
        Slot = Decals.Add(Decal);
        Residues.AddDefaulted();
    }
    if (Slot == INDEX_NONE)
    {
        return;
    }

    UDecalComponent* Decal = Decals[Slot];
    auto* Material = Cast<UMaterialInstanceDynamic>(Decal->GetDecalMaterial());
    if (!Material || Material->GetMaterial() != Source->GetMaterial())
    {
        Material = UMaterialInstanceDynamic::Create(Source->GetMaterial(), Decal);
    }
    if (Mask)
    {
        Material->SetTextureParameterValue(TEXT("BloodMask"), Mask);
    }
    auto& Residue = Residues[Slot];
    Residue.Surface = Surface;
    Residue.Normal = Normal;
    Residue.CreatedAt = Now;
    Residue.Kind = Kind;
    Residue.Coverage = Kind == 2 ? 0.4f : 1.0f;
    Residue.Lifetime = Kind == 1 ? Settings->DropletLifetime : Settings->DecalLifetime;
    Material->SetScalarParameterValue(TEXT("Coverage"), Residue.Coverage);
    Material->SetScalarParameterValue(TEXT("SpawnTime"), Now);
    Material->SetScalarParameterValue(TEXT("MirrorU"), Random.RandRange(0, 1));
    Decal->SetDecalMaterial(Material);
    Decal->DecalSize = FVector(2.0f, HalfLength, HalfWidth);
    Decal->SetWorldLocationAndRotation(Contact.ImpactPoint + Normal * 0.25f, FRotationMatrix::MakeFromXY(-Normal, Axis).Rotator());
    Decal->SetFadeOut(Residue.Lifetime, 4.0f, false);
    Decal->SetLifeSpan(0.0f);
    Decal->SetVisibility(true);
    Decal->MarkRenderStateDirty();
}
