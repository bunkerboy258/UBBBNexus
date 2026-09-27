#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"

#include "MassCommonFragments.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelAccessor.h"
#include "NiagaraDataChannel.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"

void FBBBProjectilePresentation::Publish(UWorld& World, TConstArrayView<FTransformFragment> Transforms,
    TConstArrayView<FBBBProjectileMotionFragment> Motion, TConstArrayView<FBBBProjectilePresentationFragment> Presentation)
{
    TMap<TTuple<UNiagaraDataChannelAsset*, FIntVector>, TArray<int32>> Batches;
    for (int32 Index = 0; Index < Transforms.Num(); ++Index)
    {
        if (Motion[Index].bInitialized && Presentation[Index].Channel.IsValid())
        {
            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FIntVector Cell(FMath::FloorToInt(Location.X / 10000.0),
                FMath::FloorToInt(Location.Y / 10000.0), FMath::FloorToInt(Location.Z / 10000.0));
            Batches.FindOrAdd(MakeTuple(Presentation[Index].Channel.Get(), Cell)).Add(Index);
        }
    }

    for (const auto& Batch : Batches)
    {
        UNiagaraDataChannelAsset* Asset = Batch.Key.Get<0>();
        UNiagaraDataChannel* Channel = Asset->Get();
        if (!ensureMsgf(Channel != nullptr, TEXT("子弹光效通道尚未配置")))
        {
            continue;
        }

        FNDCAccessContextInst AccessContext(Channel->GetAccessContextType());
        if (auto* Legacy = AccessContext.Get<FNDCAccessContextLegacy>())
        {
            Legacy->Location = FVector(Batch.Key.Get<1>()) * 10000.0 + FVector(5000.0);
            Legacy->bOverrideLocation = true;
        }
        UNiagaraDataChannelWriter* Writer = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel_WithContext(
            &World, Asset, AccessContext, Batch.Value.Num(), false, true, false, TEXT("BBBMassProjectile"));
        if (Writer == nullptr)
        {
            continue;
        }

        for (int32 Output = 0; Output < Batch.Value.Num(); ++Output)
        {
            const int32 Index = Batch.Value[Output];
            const FVector End = Transforms[Index].GetTransform().GetLocation();
            const FVector Delta = End - Motion[Index].PreviousLocation;
            const FVector Direction = Delta.GetSafeNormal();
            const double Length = FMath::Min(Delta.Size(), 250.0);
            Writer->WritePosition(TEXT("Position"), Output, End - Direction * Length * 0.5);
            Writer->WriteVector(TEXT("SpriteAlignment"), Output, Direction);
            Writer->WriteVector2D(TEXT("SpriteSize"), Output, FVector2D(2.5, FMath::Max(Length, 2.5)));
            Writer->WriteLinearColor(TEXT("Color"), Output, FLinearColor(20.0f, 8.0f, 1.0f, 1.0f));
        }
    }
}
