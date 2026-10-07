#include "BBBMonsterBloodPresentationSubsystem.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBloodPresentationDefinition.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "NiagaraDataChannel.h"
#include "NiagaraDataChannelAccessor.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelHandler.h"
#include "NiagaraDataChannelData.h"
#include "Misc/App.h"

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
    DecalTimes.Reset();
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
                const FVector Spray = (Outward * 0.85f + Tangent * 0.3f + FVector::UpVector * 0.15f).GetSafeNormal();
                Writer->WriteVector(TEXT("Normal"), Index, Outward);
                Writer->WriteVector(TEXT("Direction"), Index, Spray);
                Writer->WriteInt(TEXT("Surface"), Index, 2);
            }
        }
    }
    if (Settings.GroundMaterials.IsEmpty())
    {
        return;
    }
    const double Now = World->GetTimeSeconds();
    for (const FBBBMonsterBloodImpact& Impact : Impacts)
    {
        const FVector Outward = Impact.Normal.GetSafeNormal();
        const FVector Spray = (Outward * 0.85f + FVector::VectorPlaneProject(Impact.Direction, Outward).GetSafeNormal() * 0.3f
            + FVector::UpVector * 0.15f).GetSafeNormal();
        const FVector Start = Impact.Position + Outward * 4.0f;
        FHitResult Ground;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterBloodGround), false);
        bool bHitSurface = World->LineTraceSingleByChannel(Ground, Start, Start + Spray * 180.0f, ECC_WorldStatic, Params);
        if (!bHitSurface)
        {
            const FVector DropletEnd = Start + Spray * 90.0f - FVector::UpVector * 260.0f;
            bHitSurface = World->LineTraceSingleByChannel(Ground, Start, DropletEnd, ECC_WorldStatic, Params);
        }
        if (!bHitSurface || Ground.bStartPenetrating || Ground.ImpactNormal.Z < -0.1f)
        {
            continue;
        }
        bool bNearby = false;
        int32 Slot = INDEX_NONE;
        double Oldest = Now;
        for (int32 Index = 0; Index < Decals.Num(); ++Index)
        {
            if (Now - DecalTimes[Index] < Settings.DecalLifetime
                && FVector::DistSquared(Decals[Index]->GetComponentLocation(), Ground.ImpactPoint) < FMath::Square(Settings.DecalSpacing)
                && FVector::DotProduct(-Decals[Index]->GetForwardVector(), Ground.ImpactNormal) > 0.8f)
            {
                bNearby = true;
            }
            if (DecalTimes[Index] < Oldest)
            {
                Oldest = DecalTimes[Index];
                Slot = Index;
            }
        }
        if (bNearby)
        {
            continue;
        }
        if (Decals.Num() < FMath::Clamp(Settings.MaximumDecals, 1, 256))
        {
            auto* Decal = NewObject<UDecalComponent>(this);
            Decal->SetFadeScreenSize(0.002f);
            Decal->RegisterComponentWithWorld(World);
            Slot = Decals.Add(Decal);
            DecalTimes.Add(0.0);
        }
        if (Slot == INDEX_NONE)
        {
            continue;
        }
        UDecalComponent* Decal = Decals[Slot];
        const int32 Choice = (FMath::Abs(FMath::RoundToInt(Ground.ImpactPoint.X)) + Slot) % Settings.GroundMaterials.Num();
        Decal->SetDecalMaterial(Settings.GroundMaterials[Choice]);
        const float Size = Impact.Region == EBBBMonsterHitRegion::Head ? 22.0f : 17.0f;
        Decal->DecalSize = FVector(5.0f, Size, Size * 1.3f);
        FRotator Rotation = (-Ground.ImpactNormal).Rotation();
        Rotation.Roll = FMath::Fmod(Impact.Direction.Rotation().Yaw + Slot * 47.0f, 360.0f);
        Decal->SetWorldLocationAndRotation(Ground.ImpactPoint + Ground.ImpactNormal * 0.5f, Rotation);
        Decal->SetFadeOut(Settings.DecalLifetime, 4.0f, false);
        Decal->SetLifeSpan(0.0f);
        Decal->MarkRenderStateDirty();
        DecalTimes[Slot] = Now;
    }
}
